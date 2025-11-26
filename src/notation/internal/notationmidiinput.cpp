/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore Limited
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "notationmidiinput.h"

#include <QGuiApplication>

#include "engraving/dom/masterscore.h"
#include "engraving/dom/segment.h"
#include "engraving/dom/tie.h"
#include "engraving/dom/score.h"
#include "engraving/dom/note.h"
#include "engraving/dom/chord.h"
#include "engraving/dom/rest.h"
#include "engraving/dom/factory.h"
#include "engraving/dom/sig.h"
#include "engraving/dom/tempo.h"
#include "engraving/dom/measure.h"

#include "notationtypes.h"
#include "notequantizer.h"

#include "defer.h"
#include "log.h"

using namespace mu::notation;

static constexpr int PROCESS_INTERVAL = 20;

static mu::playback::IPlaybackController::PlayParams makeNoteOnParams(bool infiniteDuration)
{
    mu::playback::IPlaybackController::PlayParams params;
    params.flushSound = !infiniteDuration;

    if (infiniteDuration) {
        params.duration = muse::mpe::INFINITE_DURATION; // play note on only
    }

    return params;
}

static mu::playback::IPlaybackController::PlayParams makeNoteOffParams()
{
    mu::playback::IPlaybackController::PlayParams params;
    params.flushSound = false;
    params.duration = 0; // note off only

    return params;
}

NotationMidiInput::NotationMidiInput(IGetScore* getScore, INotationInteractionPtr notationInteraction,
                                     INotationUndoStackPtr undoStack, const muse::modularity::ContextPtr& iocCtx)
    : muse::Injectable(iocCtx), m_getScore(getScore),
    m_notationInteraction(notationInteraction), m_undoStack(undoStack)
{
    QObject::connect(&m_processTimer, &QTimer::timeout, [this]() { doProcessEvents(); });

    m_realtimeTimer.setTimerType(Qt::PreciseTimer);
    QObject::connect(&m_realtimeTimer, &QTimer::timeout, [this]() { doRealtimeAdvance(); });

    m_extendNoteTimer.setTimerType(Qt::PreciseTimer);
    m_extendNoteTimer.setSingleShot(true);
    QObject::connect(&m_extendNoteTimer, &QTimer::timeout, [this]() { doExtendCurrentNote(); });

    // Initialize recording clock
    m_recordingClock = std::make_unique<RecordingClock>();

    m_recordingClock->metronomeClick().onReceive(this, [this](int tick) {
        onRecordingMetronomeClick(tick);
    });

    m_recordingClock->beatChanged().onReceive(this, [this](int beat) {
        onRecordingBeatChanged(beat);
    });

    // Initialize keyboard chord detection timer
    m_keyboardChordTimer.setSingleShot(true);
    m_keyboardChordTimer.setInterval(KEYBOARD_CHORD_WINDOW_MS);
    QObject::connect(&m_keyboardChordTimer, &QTimer::timeout, [this]() {
        processKeyboardChordBuffer();
    });
}

void NotationMidiInput::onMidiEventReceived(const muse::midi::Event& event)
{
    if (event.isChannelVoice20()) {
        auto events = event.toMIDI10();
        for (auto& midi10event : events) {
            onMidiEventReceived(midi10event);
        }

        return;
    }

    const static std::unordered_set<muse::midi::Event::Opcode> ACCEPTED_OPCODES {
        muse::midi::Event::Opcode::NoteOn,
        muse::midi::Event::Opcode::NoteOff,
        muse::midi::Event::Opcode::ControlChange,
        muse::midi::Event::Opcode::PitchBend,
    };

    if (muse::contains(ACCEPTED_OPCODES, event.opcode())) {
        // Capture tick timestamp at arrival time for quantized recording
        int tickTimestamp = 0;
        if (m_isQuantizedRecording && m_recordingClock && m_recordingClock->isRunning()) {
            tickTimestamp = m_recordingClock->currentTick();
            LOGD() << "MIDI event arrived: opcode=" << static_cast<int>(event.opcode())
                   << " note=" << event.note() << " tickTimestamp=" << tickTimestamp
                   << " elapsedMs=" << m_recordingClock->elapsedMs();

            // NOTE: Audio preview during quantized recording is intentionally disabled.
            // MuseScore's audio pipeline has ~30-50ms latency which makes real-time
            // monitoring unusable for recording. Users should use external MIDI monitoring
            // (their keyboard's built-in sounds, a DAW, or standalone synth software)
            // for low-latency audio feedback during recording.
        }

        m_eventsQueue.push_back({ event, tickTimestamp });

        if (!m_processTimer.isActive()) {
            m_processTimer.start(PROCESS_INTERVAL);
        }
    }
}

muse::async::Channel<std::vector<const Note*> > NotationMidiInput::notesReceived() const
{
    return m_notesReceivedChannel;
}

void NotationMidiInput::onRealtimeAdvance()
{
    if (!isNoteInputMode()) {
        return;
    }

    if (isRealtimeManual()) {
        m_allowRealtimeRests = true;
        enableMetronome();
        doRealtimeAdvance();
    } else if (isRealtimeAuto()) {
        if (m_realtimeTimer.isActive()) {
            stopRealtime();
            disableMetronome();
        } else {
            m_allowRealtimeRests = true;
            enableMetronome();
            runRealtime();
        }
    }
}

mu::engraving::Score* NotationMidiInput::score() const
{
    IF_ASSERT_FAILED(m_getScore) {
        return nullptr;
    }

    return m_getScore->score();
}

void NotationMidiInput::doProcessEvents()
{
    DEFER {
        m_eventsQueue.clear();
        m_processTimer.stop();
    };

    if (m_eventsQueue.empty()) {
        return;
    }

    const mu::engraving::Score* sc = score();
    if (!sc || sc->noStaves()) {
        return;
    }

    std::vector<const Note*> notesOn;
    std::vector<int> notesOff;
    ControllerEventMap controllers;

    startNoteInputIfNeed();
    bool isNoteInput = isNoteInputMode();
    bool isSoundPreview = !isNoteInput;

    if (isNoteInput && isInputByDuration()) {
        addNoteEventsToInputState();
        return;
    }

    // Route to quantized recording if active
    if (m_isQuantizedRecording && m_recordingClock && m_recordingClock->isRunning()) {
        for (const TimestampedMidiEvent& timestampedEvent : m_eventsQueue) {
            const muse::midi::Event& event = timestampedEvent.event;
            int tick = timestampedEvent.tickTimestamp;
            const muse::midi::Event::Opcode opcode = event.opcode();

            if (opcode == muse::midi::Event::Opcode::NoteOn && event.velocity7() > 0) {
                bufferNoteOn(event.note(), event.velocity7(), tick);
            } else if (opcode == muse::midi::Event::Opcode::NoteOff ||
                       (opcode == muse::midi::Event::Opcode::NoteOn && event.velocity7() == 0)) {
                bufferNoteOff(event.note(), tick);
            }
        }
        return; // Don't process events in the normal way during quantized recording
    }

    const bool useDurationAndVelocity = isSoundPreview || configuration()->useMidiVelocityAndDurationDuringNoteInput();

    for (size_t i = 0; i < m_eventsQueue.size(); ++i) {
        const muse::midi::Event& event = m_eventsQueue.at(i).event;
        const muse::midi::Event::Opcode opcode = event.opcode();

        if (opcode == muse::midi::Event::Opcode::ControlChange || opcode == muse::midi::Event::Opcode::PitchBend) {
            controllers[opcode] = event; // keep only last received to prevent spam
            continue;
        }

        Note* note = isNoteInput ? addNoteToScore(event) : makePreviewNote(event);
        if (note) {
            if (useDurationAndVelocity) {
                note->setUserVelocity(event.velocity7());
                m_playingNotes[note->pitch()] = { isSoundPreview, note };
            }
            notesOn.push_back(note);
        }

        const bool chord = i != 0;
        const bool noteOn = opcode == muse::midi::Event::Opcode::NoteOn;
        if (!chord && noteOn && !m_realtimeTimer.isActive() && isRealtimeAuto()) {
            m_extendNoteTimer.start(configuration()->delayBetweenNotesInRealTimeModeMilliseconds());
            enableMetronome();
            doRealtimeAdvance();
        }

        const bool noteOff = opcode == muse::midi::Event::Opcode::NoteOff || event.velocity7() == 0;
        if (useDurationAndVelocity && noteOff) {
            notesOff.push_back(event.note());
        }
    }

    if (!controllers.empty()) {
        triggerControllers(controllers);
    }

    if (!notesOn.empty()) {
        const std::vector<const EngravingItem*> elements(notesOn.begin(), notesOn.end());
        playbackController()->seekElement(notesOn.front(), !useDurationAndVelocity /*flushSound*/);
        playbackController()->playElements(elements, makeNoteOnParams(useDurationAndVelocity), true);
        m_notesReceivedChannel.send(notesOn);
    }

    if (!notesOff.empty()) {
        releasePlayingNotes(notesOff);
    }
}

void NotationMidiInput::startNoteInputIfNeed()
{
    if (isNoteInputMode()) {
        return;
    }

    const auto containsNoteOn = [this]() -> bool {
        return std::any_of(m_eventsQueue.begin(), m_eventsQueue.end(), [](const TimestampedMidiEvent& te) {
            return te.event.opcode() == muse::midi::Event::Opcode::NoteOn;
        });
    };

    if (configuration()->startNoteInputAtSelectedNoteRestWhenPressingMidiKey()) {
        if (m_notationInteraction->selection()->elementsSelected(NOTE_REST_TYPES) && containsNoteOn()) {
            dispatcher()->dispatch("note-input");
        }
    }
}

void NotationMidiInput::addNoteEventsToInputState()
{
    INotationNoteInputPtr noteInput = m_notationInteraction->noteInput();
    const NoteInputState& state = noteInput->state();
    const staff_idx_t staffIdx = state.staffIdx();
    const bool useWrittenPitch = configuration()->midiUseWrittenPitch().val;
    const bool playPreviewNotes = configuration()->isPlayPreviewNotesInInputByDuration();
    const bool useVelocityAndDuration = playPreviewNotes && configuration()->useMidiVelocityAndDurationDuringNoteInput();

    NoteValList notesOn;
    NoteValList notesOff;

    if (m_holdingNotesInInputByDuration) {
        notesOn = state.notes();
    }

    ControllerEventMap controllers;

    for (const TimestampedMidiEvent& timestampedEvent : m_eventsQueue) {
        const muse::midi::Event& event = timestampedEvent.event;
        const muse::midi::Event::Opcode opcode = event.opcode();

        if (opcode == muse::midi::Event::Opcode::NoteOn) {
            NoteVal nval = score()->noteVal(event.note(), staffIdx, useWrittenPitch);
            nval.velocityOverride = event.velocity7();
            notesOn.push_back(nval);
            m_holdingNotesInInputByDuration = true;
        } else if (opcode == muse::midi::Event::Opcode::NoteOff) {
            if (useVelocityAndDuration) {
                notesOff.push_back(score()->noteVal(event.note(), staffIdx, useWrittenPitch));
            }
            m_holdingNotesInInputByDuration = false;
        } else if (opcode == muse::midi::Event::Opcode::ControlChange || opcode == muse::midi::Event::Opcode::PitchBend) {
            if (playPreviewNotes) {
                controllers[opcode] = event; // keep only last received to prevent spam
            }
        }
    }

    if (!controllers.empty()) {
        triggerControllers(controllers);
    }

    if (!notesOff.empty()) {
        playbackController()->playNotes(notesOff, staffIdx, state.segment(), makeNoteOffParams());
    }

    if (!notesOn.empty() && notesOn != state.notes()) {
        noteInput->setRestMode(false);
        noteInput->setInputNotes(notesOn);

        if (playPreviewNotes) {
            playbackController()->playNotes(notesOn, staffIdx, state.segment(), makeNoteOnParams(useVelocityAndDuration));
        }
    }
}

Note* NotationMidiInput::addNoteToScore(const muse::midi::Event& e)
{
    mu::engraving::Score* sc = score();

    mu::engraving::MidiInputEvent inputEv;
    inputEv.pitch = e.note();
    inputEv.velocity = e.velocity7();

    sc->activeMidiPitches().remove_if([&inputEv](const mu::engraving::MidiInputEvent& val) {
        return inputEv.pitch == val.pitch;
    });

    const mu::engraving::InputState& is = sc->inputState();
    if (!is.noteEntryMode()) {
        return nullptr;
    }

    if (!is.isValid()) {
        return nullptr;
    }

    DEFER {
        m_undoStack->commitChanges();
    };

    m_undoStack->prepareChanges(muse::TranslatableString("undoableAction", "Enter note"));

    if (e.opcode() == muse::midi::Event::Opcode::NoteOff) {
        if (isRealtime()) {
            if (!is.cr()) {
                return nullptr;
            }

            const Chord* chord = is.cr()->isChord() ? engraving::toChord(is.cr()) : nullptr;
            if (chord) {
                Note* n = chord->findNote(inputEv.pitch);
                if (n) {
                    sc->deleteItem(n->tieBack());
                    sc->deleteItem(n);
                }
            }
        }

        return nullptr;
    }

    if (sc->activeMidiPitches().empty()) {
        inputEv.chord = false;
    } else {
        inputEv.chord = true;
    }

    // holding shift while inputting midi will add the new pitch to the prior existing chord
    if (QGuiApplication::queryKeyboardModifiers() & Qt::ShiftModifier) {
        mu::engraving::EngravingItem* cr = is.lastSegment()->element(is.track());
        if (cr && cr->isChord()) {
            inputEv.chord = true;
        }
    }

    mu::engraving::Note* note = sc->addMidiPitch(inputEv.pitch, inputEv.chord, configuration()->midiUseWrittenPitch().val);

    sc->activeMidiPitches().push_back(inputEv);

    if (is.cr()) {
        m_notationInteraction->showItem(is.cr());
    }

    return note;
}

Note* NotationMidiInput::makePreviewNote(const muse::midi::Event& e)
{
    if (e.opcode() == muse::midi::Event::Opcode::NoteOff || e.velocity7() == 0) {
        return nullptr;
    }

    mu::engraving::Score* score = this->score();
    const mu::engraving::InputState& inputState = score->inputState();
    Segment* seg = inputState.lastSegment() ? inputState.lastSegment() : score->dummy()->segment();

    Chord* chord = engraving::Factory::createChord(seg);
    chord->setParent(seg);

    const ChordRest* cr = inputState.cr();
    const mu::engraving::staff_idx_t staffIdx = cr ? engraving::track2staff(cr->track()) : 0;

    Note* note = engraving::Factory::createNote(chord);
    note->setParent(chord);
    note->setStaffIdx(staffIdx);

    engraving::NoteVal nval = score->noteVal(e.note(), staffIdx, configuration()->midiUseWrittenPitch().val);
    note->setNval(nval);

    return note;
}

void NotationMidiInput::triggerControllers(const ControllerEventMap& events)
{
    muse::mpe::ControllerChangeEventList controllers;

    static const std::unordered_map<int, muse::mpe::ControllerChangeEvent::Type> MIDI_CC_TO_EVENT_TYPE {
        { muse::midi::MODWHEEL_CONTROLLER, muse::mpe::ControllerChangeEvent::Modulation },
        { muse::midi::SUSTAIN_PEDAL_CONTROLLER, muse::mpe::ControllerChangeEvent::SustainPedalOnOff },
    };

    const mu::engraving::InputState& is = score()->inputState();

    for (const auto& pair : events) {
        const muse::midi::Event& e = pair.second;
        muse::mpe::ControllerChangeEvent cc;

        if (pair.first == muse::midi::Event::Opcode::PitchBend) {
            cc.type = muse::mpe::ControllerChangeEvent::PitchBend;
            cc.val = static_cast<float>(e.pitchBend14()) / 16383.f;
        } else {
            cc.type = muse::value(MIDI_CC_TO_EVENT_TYPE, e.index(), muse::mpe::ControllerChangeEvent::Undefined);
            cc.val = static_cast<float>(e.data()) / 127.f;
        }

        if (cc.type != muse::mpe::ControllerChangeEvent::Undefined) {
            cc.layerIdx = static_cast<muse::mpe::layer_idx_t>(is.track());
            controllers.push_back(cc);
        }
    }

    playbackController()->triggerControllers(controllers, is.staffIdx(), is.tick().ticks());
}

void NotationMidiInput::releasePlayingNotes(const std::vector<int>& pitches)
{
    std::vector<const EngravingItem*> notesOff;
    std::vector<Note*> notesToDelete;

    const staff_idx_t staffIdx = score()->inputState().staffIdx();
    const bool useWrittenPitch = configuration()->midiUseWrittenPitch().val;

    for (int pitch : pitches) {
        const NoteVal nval = score()->noteVal(pitch, staffIdx, useWrittenPitch);

        auto it = m_playingNotes.find(nval.pitch);
        if (it == m_playingNotes.end()) {
            continue;
        }

        Note* note = it->second.note;
        note->setUserVelocity(0);

        notesOff.push_back(note);

        if (it->second.isPreview) {
            notesToDelete.push_back(note);
        }

        m_playingNotes.erase(it);
    }

    playbackController()->playElements(notesOff, makeNoteOffParams(), true /*isMidi*/);
    muse::DeleteAll(notesToDelete);
}

void NotationMidiInput::enableMetronome()
{
    bool metronomeEnabled = configuration()->isMetronomeEnabled();
    if (metronomeEnabled) {
        return;
    }

    dispatcher()->dispatch("metronome");
    m_shouldDisableMetronome = true;
}

void NotationMidiInput::disableMetronome()
{
    if (!m_shouldDisableMetronome) {
        return;
    }

    dispatcher()->dispatch("metronome");

    m_shouldDisableMetronome = false;
}

void NotationMidiInput::runRealtime()
{
    m_realtimeTimer.start(configuration()->delayBetweenNotesInRealTimeModeMilliseconds());
}

void NotationMidiInput::stopRealtime()
{
    m_realtimeTimer.stop();
}

void NotationMidiInput::doRealtimeAdvance()
{
    if (!isRealtime() || !isNoteInputMode() || (!m_allowRealtimeRests && m_getScore->score()->activeMidiPitches().empty())) {
        if (m_realtimeTimer.isActive()) {
            stopRealtime();
        }

        disableMetronome();

        m_allowRealtimeRests = true;
        return;
    }

    const mu::engraving::InputState& is = m_getScore->score()->inputState();
    playbackController()->playMetronome(is.tick().ticks());

    QTimer::singleShot(100, Qt::PreciseTimer, [this]() {
        m_undoStack->prepareChanges(muse::TranslatableString("undoableAction", "Realtime advance"));
        m_getScore->score()->realtimeAdvance(configuration()->midiUseWrittenPitch().val);
        m_undoStack->commitChanges();
    });

    if (isRealtimeManual()) {
        int metronomeDuration = 500;
        QTimer::singleShot(metronomeDuration, Qt::PreciseTimer, [this]() {
            disableMetronome();
        });
    }
}

void NotationMidiInput::doExtendCurrentNote()
{
    if (!isNoteInputMode() || m_realtimeTimer.isActive()) {
        return;
    }

    m_allowRealtimeRests = false;
    runRealtime();
    doRealtimeAdvance();
}

NoteInputMethod NotationMidiInput::noteInputMethod() const
{
    return m_notationInteraction->noteInput()->state().noteEntryMethod();
}

bool NotationMidiInput::isRealtime() const
{
    return isRealtimeAuto() || isRealtimeManual();
}

bool NotationMidiInput::isRealtimeAuto() const
{
    return noteInputMethod() == NoteInputMethod::REALTIME_AUTO;
}

bool NotationMidiInput::isRealtimeManual() const
{
    return noteInputMethod() == NoteInputMethod::REALTIME_MANUAL;
}

bool NotationMidiInput::isInputByDuration() const
{
    return noteInputMethod() == NoteInputMethod::BY_DURATION;
}

bool NotationMidiInput::isNoteInputMode() const
{
    return m_notationInteraction->noteInput()->isNoteInputMode();
}

bool NotationMidiInput::isRealtimeQuantized() const
{
    return noteInputMethod() == NoteInputMethod::REALTIME_QUANTIZED;
}

INotationNoteInputPtr NotationMidiInput::noteInput() const
{
    return m_notationInteraction->noteInput();
}

void NotationMidiInput::setNotationPlayback(INotationPlaybackPtr playback)
{
    m_notationPlayback = playback;
}

INotationPlaybackPtr NotationMidiInput::notationPlayback() const
{
    return m_notationPlayback;
}

void NotationMidiInput::startQuantizedRecording()
{
    if (m_isQuantizedRecording) {
        return;
    }

    INotationNoteInputPtr ni = noteInput();
    if (!ni) {
        return;
    }

    // Ensure we're in note input mode with REALTIME_QUANTIZED
    if (!ni->isNoteInputMode()) {
        ni->startNoteInput(NoteInputMethod::REALTIME_QUANTIZED);
    } else if (!ni->usingNoteInputMethod(NoteInputMethod::REALTIME_QUANTIZED)) {
        ni->setNoteInputMethod(NoteInputMethod::REALTIME_QUANTIZED);
    }

    // Get tempo - use recording tempo if set, otherwise score tempo
    int recordingTempoBpm = configuration()->recordingTempo();
    double tempoBPS;

    mu::engraving::Score* sc = score();
    if (!sc) {
        return;
    }

    const NoteInputState& inputState = ni->state();
    if (!inputState.segment()) {
        return;
    }

    int currentTick = inputState.segment()->tick().ticks();

    if (recordingTempoBpm > 0) {
        tempoBPS = recordingTempoBpm / 60.0;
    } else {
        // Get tempo from score at current position
        tempoBPS = sc->tempomap()->multipliedTempo(currentTick).val;
    }

    // Get time signature info - use TimeSigFrac's built-in methods
    engraving::TimeSigFrac timeSig = sc->sigmap()->timesig(engraving::Fraction::fromTicks(currentTick)).nominal();
    int ticksPerBeat = timeSig.beatTicks();
    int beatsPerMeasure = timeSig.beatsPerMeasure();

    m_isQuantizedRecording = true;
    m_pendingNotes.clear();
    m_currentRecordingBeat = 0;
    m_lastQuantizedNoteEndTick = currentTick;

    // Enable metronome if not already enabled (same as existing real-time modes)
    enableMetronome();

    // Use the existing count-in infrastructure for proper timing
    muse::secs_t countInDurationSecs = 0;
    if (m_notationPlayback) {
        m_notationPlayback->triggerCountIn(currentTick, countInDurationSecs);
    }

    // Convert seconds to milliseconds for the timer
    int countInDurationMs = static_cast<int>(countInDurationSecs * 1000.0);
    if (countInDurationMs <= 0) {
        // Fallback: calculate manually if triggerCountIn doesn't return duration
        int countInBeats = beatsPerMeasure;
        countInDurationMs = static_cast<int>(countInBeats * 1000.0 / tempoBPS);
    }

    LOGD() << "startQuantizedRecording: countInDuration=" << countInDurationMs << "ms"
           << " tempoBPS=" << tempoBPS << " ticksPerBeat=" << ticksPerBeat;

    // After count-in, start actual recording
    QTimer::singleShot(countInDurationMs, [this, currentTick, tempoBPS, ticksPerBeat, beatsPerMeasure]() {
        if (m_isQuantizedRecording) {
            m_recordingClock->start(currentTick, tempoBPS, ticksPerBeat, beatsPerMeasure);
            LOGD() << "Recording clock started at tick=" << currentTick;
        }
    });
}

void NotationMidiInput::stopQuantizedRecording()
{
    if (!m_isQuantizedRecording) {
        return;
    }

    LOGD() << "stopQuantizedRecording called";

    // Stop the clock first (this makes isRunning() return false)
    if (m_recordingClock && m_recordingClock->isRunning()) {
        m_recordingClock->stop();
    }

    // Finalize any pending notes
    finalizeRecording();

    // Clear all buffers to ensure clean state
    m_pendingNotes.clear();
    m_recordedNotes.clear();
    m_keyboardNoteBuffer.clear();
    m_heldKeyboardPitches.clear();

    m_isQuantizedRecording = false;
    m_currentBeatChanged.send(-1); // Signal recording stopped

    // Disable metronome if we enabled it
    disableMetronome();

    LOGD() << "stopQuantizedRecording complete";
}

bool NotationMidiInput::isQuantizedRecording() const
{
    return m_isQuantizedRecording;
}

muse::async::Channel<int> NotationMidiInput::currentBeatChanged() const
{
    return m_currentBeatChanged;
}

void NotationMidiInput::onRecordingMetronomeClick(int tick)
{
    playbackController()->playMetronome(tick);
}

void NotationMidiInput::onRecordingBeatChanged(int beat)
{
    m_currentRecordingBeat = beat;
    m_currentBeatChanged.send(beat);
}

void NotationMidiInput::bufferNoteOn(int pitch, int velocity, int tickTimestamp)
{
    LOGD() << "bufferNoteOn: pitch=" << pitch << " velocity=" << velocity
           << " tickTimestamp=" << tickTimestamp;

    // Check if this pitch is already pending (shouldn't happen, but safety)
    auto it = std::find_if(m_pendingNotes.begin(), m_pendingNotes.end(),
                           [pitch](const PendingNote& n) { return n.pitch == pitch; });

    if (it != m_pendingNotes.end()) {
        // Note already pending - end the previous one first
        bufferNoteOff(pitch, tickTimestamp);
    }

    // Add to pending notes (waiting for note-off)
    m_pendingNotes.push_back({ pitch, velocity, tickTimestamp });

    // NOTE: Audio preview is now handled at the entry points (onMidiEventReceived/processKeyboardNoteOn)
    // for immediate feedback - no longer called here to avoid double-triggering
}

void NotationMidiInput::bufferNoteOff(int pitch, int tickTimestamp)
{
    LOGD() << "bufferNoteOff: pitch=" << pitch << " tickTimestamp=" << tickTimestamp;

    // Find the pending note
    auto it = std::find_if(m_pendingNotes.begin(), m_pendingNotes.end(),
                           [pitch](const PendingNote& n) { return n.pitch == pitch; });

    if (it == m_pendingNotes.end()) {
        return; // No pending note for this pitch
    }

    PendingNote pending = *it;
    m_pendingNotes.erase(it);

    // Add to recorded notes buffer (will be written to score when recording stops)
    m_recordedNotes.push_back({ pending.pitch, pending.velocity, pending.startTick, tickTimestamp });

    LOGD() << "Note buffered: pitch=" << pending.pitch
           << " startTick=" << pending.startTick
           << " endTick=" << tickTimestamp;

    // NOTE: Audio preview off is now handled at the entry points (onMidiEventReceived/processKeyboardNoteOff)
    // for immediate feedback - no longer called here to avoid double-triggering
}

void NotationMidiInput::playNotePreview(int pitch, int velocity, bool noteOn)
{
    // Create a temporary note for audio preview
    mu::engraving::Score* sc = score();
    if (!sc) {
        return;
    }

    const mu::engraving::InputState& inputState = sc->inputState();
    Segment* seg = inputState.lastSegment() ? inputState.lastSegment() : sc->dummy()->segment();

    if (!seg) {
        return;
    }

    // Create a temporary chord and note for playback
    Chord* chord = engraving::Factory::createChord(seg);
    chord->setParent(seg);

    const ChordRest* cr = inputState.cr();
    const mu::engraving::staff_idx_t staffIdx = cr ? engraving::track2staff(cr->track()) : 0;

    Note* note = engraving::Factory::createNote(chord);
    note->setParent(chord);
    note->setStaffIdx(staffIdx);

    engraving::NoteVal nval = sc->noteVal(pitch, staffIdx, configuration()->midiUseWrittenPitch().val);
    note->setNval(nval);

    if (noteOn) {
        note->setUserVelocity(velocity);
        std::vector<const EngravingItem*> elements = { note };
        playback::IPlaybackController::PlayParams params;
        params.flushSound = false; // Don't flush - we want the note to sustain
        playbackController()->playElements(elements, params, true);
    } else {
        // Note off - flush sound to stop the note
        playback::IPlaybackController::PlayParams params;
        params.flushSound = true;
        playbackController()->playElements({}, params, true);
    }

    // Clean up temporary objects
    delete chord; // This also deletes the note since it's parented to chord
}

void NotationMidiInput::finalizeRecording()
{
    // End any notes still being held
    if (!m_recordingClock) {
        return;
    }

    int endTick = m_recordingClock->currentTick();

    // Buffer any still-held notes with the current end time
    for (const auto& pending : m_pendingNotes) {
        m_recordedNotes.push_back({ pending.pitch, pending.velocity, pending.startTick, endTick });
    }
    m_pendingNotes.clear();

    // Now write all buffered notes to the score with quantization
    writeBufferedNotesToScore();
}

void NotationMidiInput::writeBufferedNotesToScore()
{
    if (m_recordedNotes.empty()) {
        LOGD() << "writeBufferedNotesToScore: no notes to write";
        return;
    }

    if (!m_recordingClock) {
        LOGW() << "writeBufferedNotesToScore: no recording clock";
        m_recordedNotes.clear();
        return;
    }

    LOGD() << "writeBufferedNotesToScore: writing " << m_recordedNotes.size() << " notes";

    // Get quantization parameters
    int ticksPerBeat = m_recordingClock->ticksPerBeat();
    int ticksPerGrid = NoteQuantizer::gridToTicks(configuration()->quantizationGrid(), ticksPerBeat);

    // Sort notes by start time
    std::sort(m_recordedNotes.begin(), m_recordedNotes.end(),
              [](const RecordedNote& a, const RecordedNote& b) {
                  return a.startTick < b.startTick;
              });

    // Write each note to the score
    for (const auto& recorded : m_recordedNotes) {
        // Quantize the start position (round to nearest)
        int quantizedStartTick = NoteQuantizer::quantizePosition(recorded.startTick, ticksPerGrid);

        // Calculate quantized duration
        engraving::Fraction duration = NoteQuantizer::calculateDuration(
            recorded.startTick, recorded.endTick, ticksPerBeat, ticksPerGrid);

        LOGD() << "Writing note: pitch=" << recorded.pitch
               << " rawStart=" << recorded.startTick << " quantizedStart=" << quantizedStartTick
               << " rawEnd=" << recorded.endTick << " duration=" << duration.toString();

        // Create the note in the score
        createQuantizedNote(recorded.pitch, recorded.velocity, quantizedStartTick, duration);
    }

    m_recordedNotes.clear();
}

void NotationMidiInput::createQuantizedNote(int pitch, int velocity, int startTick,
                                            const engraving::Fraction& duration)
{
    INotationNoteInputPtr ni = noteInput();
    if (!ni) {
        return;
    }

    mu::engraving::Score* sc = score();
    if (!sc) {
        return;
    }

    const NoteInputState& inputState = ni->state();
    if (!inputState.segment()) {
        return;
    }

    // Get current input position
    int currentTick = inputState.segment()->tick().ticks();

    // Insert rests to fill gap from current position to start of this note
    if (startTick > currentTick) {
        insertRestsToFillGap(currentTick, startTick);
    }

    // Set up the note value
    engraving::NoteVal nval;
    nval.pitch = pitch;

    // Set velocity if configured to use it
    if (configuration()->useMidiVelocityAndDurationDuringNoteInput()) {
        nval.velocityOverride = velocity;
    }

    // Get track from input state
    engraving::track_idx_t track = inputState.track();

    // Get the segment at startTick (or create one)
    engraving::Segment* seg = sc->tick2segment(engraving::Fraction::fromTicks(startTick),
                                                true, engraving::SegmentType::ChordRest);
    if (!seg) {
        // Try to find the measure and get appropriate segment
        engraving::Measure* measure = sc->tick2measure(engraving::Fraction::fromTicks(startTick));
        if (!measure) {
            LOGW() << "Cannot find measure at tick " << startTick;
            return;
        }
        seg = measure->getSegment(engraving::SegmentType::ChordRest,
                                   engraving::Fraction::fromTicks(startTick));
    }

    if (!seg) {
        LOGW() << "Cannot create segment at tick " << startTick;
        return;
    }

    // Prepare the undo operation
    m_undoStack->prepareChanges(muse::TranslatableString("undoableAction", "Record note"));

    // Create the note using the score's setNoteRest
    engraving::Segment* resultSeg = sc->setNoteRest(seg, track, nval, duration);

    m_undoStack->commitChanges();

    // Update the last note end position
    m_lastQuantizedNoteEndTick = startTick + duration.ticks();

    // Move input position to after the note
    if (resultSeg) {
        // Find next segment for input
        engraving::Segment* nextSeg = resultSeg->next(engraving::SegmentType::ChordRest);
        if (nextSeg) {
            sc->inputState().setSegment(nextSeg);
        }
    }

    // Notify about state change
    m_notationInteraction->noteInput()->stateChanged().notify();

    LOGD() << "Created quantized note: pitch=" << pitch << " startTick=" << startTick
           << " duration=" << duration.toString();
}

void NotationMidiInput::insertRestsToFillGap(int fromTick, int toTick)
{
    if (toTick <= fromTick) {
        return;
    }

    INotationNoteInputPtr ni = noteInput();
    if (!ni) {
        return;
    }

    mu::engraving::Score* sc = score();
    if (!sc) {
        return;
    }

    const NoteInputState& inputState = ni->state();
    engraving::track_idx_t track = inputState.track();

    // Calculate the gap duration
    engraving::Fraction gapDuration = engraving::Fraction::fromTicks(toTick - fromTick);

    // Get or create segment at fromTick
    engraving::Segment* seg = sc->tick2segment(engraving::Fraction::fromTicks(fromTick),
                                                true, engraving::SegmentType::ChordRest);
    if (!seg) {
        engraving::Measure* measure = sc->tick2measure(engraving::Fraction::fromTicks(fromTick));
        if (!measure) {
            return;
        }
        seg = measure->getSegment(engraving::SegmentType::ChordRest,
                                   engraving::Fraction::fromTicks(fromTick));
    }

    if (!seg) {
        return;
    }

    // Prepare the undo operation
    m_undoStack->prepareChanges(muse::TranslatableString("undoableAction", "Insert rest"));

    // Create rest using setNoteRest with pitch -1 (rest)
    engraving::NoteVal restVal;
    restVal.pitch = -1; // -1 indicates rest

    sc->setNoteRest(seg, track, restVal, gapDuration);

    m_undoStack->commitChanges();

    LOGD() << "Inserted rest: fromTick=" << fromTick << " toTick=" << toTick
           << " duration=" << gapDuration.toString();
}

void NotationMidiInput::processKeyboardNoteOn(int pitch, int velocity)
{
    if (!m_isQuantizedRecording || !m_recordingClock || !m_recordingClock->isRunning()) {
        return;
    }

    // Filter out key repeat events - if this pitch is already held, ignore
    if (m_heldKeyboardPitches.count(pitch) > 0) {
        LOGD() << "Ignoring key repeat for pitch=" << pitch;
        return;
    }

    // NOTE: Audio preview during quantized recording is intentionally disabled.
    // MuseScore's audio pipeline has ~30-50ms latency which makes real-time
    // monitoring unusable for recording. Users should use external MIDI monitoring
    // for low-latency audio feedback during recording.

    // Capture tick timestamp NOW when key is first pressed
    int tickTimestamp = m_recordingClock->currentTick();

    // Monophonic behavior: auto-release any currently held note when a new key is pressed
    // This allows overlapping keypresses where the user hits the next note before releasing the previous
    if (!m_heldKeyboardPitches.empty()) {
        // Release all currently held notes at this timestamp
        for (int heldPitch : m_heldKeyboardPitches) {
            LOGD() << "Auto-releasing held pitch=" << heldPitch << " due to new keypress";
            bufferNoteOff(heldPitch, tickTimestamp);
        }
        m_heldKeyboardPitches.clear();
    }

    // Mark this pitch as held
    m_heldKeyboardPitches.insert(pitch);

    LOGD() << "processKeyboardNoteOn: pitch=" << pitch << " velocity=" << velocity
           << " tickTimestamp=" << tickTimestamp << " elapsedMs=" << m_recordingClock->elapsedMs();

    // For keyboard input, buffer immediately (single notes only, no chord detection)
    bufferNoteOn(pitch, velocity, tickTimestamp);
}

void NotationMidiInput::processKeyboardNoteOff(int pitch)
{
    if (!m_isQuantizedRecording || !m_recordingClock || !m_recordingClock->isRunning()) {
        return;
    }

    // Clear the held state for this pitch
    m_heldKeyboardPitches.erase(pitch);

    int tick = m_recordingClock->currentTick();
    LOGD() << "processKeyboardNoteOff: pitch=" << pitch << " tickTimestamp=" << tick
           << " elapsedMs=" << m_recordingClock->elapsedMs();
    bufferNoteOff(pitch, tick);
}

void NotationMidiInput::processKeyboardNoteOffByLetter(int noteLetter)
{
    if (!m_isQuantizedRecording || !m_recordingClock || !m_recordingClock->isRunning()) {
        return;
    }

    // Find pending note matching this letter (regardless of octave)
    // Note letters: 0=C, 1=D, 2=E, 3=F, 4=G, 5=A, 6=B
    static const int pitchToLetter[12] = { 0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6 };

    int tick = m_recordingClock->currentTick();

    // Find the pitch first, then call processQuantizedNoteOff outside the loop
    // (to avoid modifying the vector while iterating)
    int foundPitch = -1;
    for (const auto& pending : m_pendingNotes) {
        int pendingLetter = pitchToLetter[pending.pitch % 12];
        if (pendingLetter == noteLetter) {
            foundPitch = pending.pitch;
            break;
        }
    }

    if (foundPitch >= 0) {
        // Clear the held state for this pitch
        m_heldKeyboardPitches.erase(foundPitch);

        LOGD() << "processKeyboardNoteOffByLetter: letter=" << noteLetter << " pitch=" << foundPitch
               << " tickTimestamp=" << tick << " elapsedMs=" << m_recordingClock->elapsedMs();
        bufferNoteOff(foundPitch, tick);
    }
}

void NotationMidiInput::processKeyboardChordBuffer()
{
    if (m_keyboardNoteBuffer.empty()) {
        return;
    }

    if (!m_recordingClock || !m_recordingClock->isRunning()) {
        m_keyboardNoteBuffer.clear();
        return;
    }

    // All notes in buffer are treated as a chord - use the earliest timestamp
    // (they should all be very close, but use the first one for consistency)
    int chordTick = m_keyboardNoteBuffer.front().tickTimestamp;

    LOGD() << "processKeyboardChordBuffer: " << m_keyboardNoteBuffer.size() << " notes, chordTick=" << chordTick;

    for (const auto& entry : m_keyboardNoteBuffer) {
        // Use the stored timestamp from when the key was pressed, not current time
        bufferNoteOn(entry.pitch, entry.velocity, entry.tickTimestamp);
    }

    m_keyboardNoteBuffer.clear();
}
