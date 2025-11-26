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
#ifndef MU_NOTATION_NOTATIONMIDIINPUT_H
#define MU_NOTATION_NOTATIONMIDIINPUT_H

#include <QTimer>
#include <memory>
#include <set>

#include "modularity/ioc.h"
#include "async/asyncable.h"
#include "playback/iplaybackcontroller.h"
#include "inotationconfiguration.h"
#include "actions/iactionsdispatcher.h"

#include "../inotationmidiinput.h"
#include "../inotationplayback.h"
#include "igetscore.h"
#include "inotationinteraction.h"
#include "inotationundostack.h"
#include "recordingclock.h"

namespace mu::engraving {
class Score;
}

namespace mu::notation {
class NotationMidiInput : public INotationMidiInput, public muse::Injectable, public muse::async::Asyncable
{
    muse::Inject<playback::IPlaybackController> playbackController = { this };
    muse::Inject<muse::actions::IActionsDispatcher> dispatcher = { this };
    muse::Inject<INotationConfiguration> configuration = { this };

public:
    NotationMidiInput(IGetScore* getScore, INotationInteractionPtr notationInteraction, INotationUndoStackPtr undoStack,
                      const muse::modularity::ContextPtr& iocCtx);

    void onMidiEventReceived(const muse::midi::Event& event) override;
    muse::async::Channel<std::vector<const Note*> > notesReceived() const override;

    void onRealtimeAdvance() override;

    // Quantized recording methods
    void startQuantizedRecording() override;
    void stopQuantizedRecording() override;
    bool isQuantizedRecording() const override;
    muse::async::Channel<int> currentBeatChanged() const override;

    // Keyboard note processing for quantized recording
    // noteLetter is 0-6 for C-B
    void processKeyboardNoteOn(int pitch, int velocity) override;
    void processKeyboardNoteOff(int pitch) override;
    void processKeyboardNoteOffByLetter(int noteLetter);

    void setNotationPlayback(INotationPlaybackPtr playback) override;
    INotationPlaybackPtr notationPlayback() const;

private:
    mu::engraving::Score* score() const;

    void doProcessEvents();

    void startNoteInputIfNeed();

    void addNoteEventsToInputState();
    Note* addNoteToScore(const muse::midi::Event& e);
    Note* makePreviewNote(const muse::midi::Event& e);

    using ControllerEventMap = std::map<muse::midi::Event::Opcode, muse::midi::Event>;
    void triggerControllers(const ControllerEventMap& events);
    void releasePlayingNotes(const std::vector<int>& pitches);

    void enableMetronome();
    void disableMetronome();

    void runRealtime();
    void stopRealtime();

    void doRealtimeAdvance();
    void doExtendCurrentNote();

    NoteInputMethod noteInputMethod() const;
    bool isRealtime() const;
    bool isRealtimeAuto() const;
    bool isRealtimeManual() const;
    bool isRealtimeQuantized() const;
    bool isInputByDuration() const;

    bool isNoteInputMode() const;

    // Quantized recording helpers
    void onRecordingMetronomeClick(int tick);
    void onRecordingBeatChanged(int beat);
    void bufferNoteOn(int pitch, int velocity, int tickTimestamp);
    void bufferNoteOff(int pitch, int tickTimestamp);
    void playNotePreview(int pitch, int velocity, bool noteOn);
    void writeBufferedNotesToScore();
    void createQuantizedNote(int pitch, int velocity, int startTick, const mu::engraving::Fraction& duration);
    void insertRestsToFillGap(int fromTick, int toTick);
    void finalizeRecording();
    void processKeyboardChordBuffer();  // For MIDI chord detection (keyboard bypasses this)

    INotationNoteInputPtr noteInput() const;

    IGetScore* m_getScore = nullptr;
    INotationInteractionPtr m_notationInteraction;
    INotationUndoStackPtr m_undoStack;
    muse::async::Channel<std::vector<const Note*> > m_notesReceivedChannel;

    QTimer m_processTimer;
    struct TimestampedMidiEvent {
        muse::midi::Event event;
        int tickTimestamp = 0; // Tick at time of arrival (for quantized recording)
    };
    std::vector<TimestampedMidiEvent> m_eventsQueue;

    QTimer m_realtimeTimer;
    QTimer m_extendNoteTimer;
    bool m_allowRealtimeRests = false;

    bool m_shouldDisableMetronome = false;
    bool m_holdingNotesInInputByDuration = false;

    struct PlayingNote {
        bool isPreview = false;
        Note* note = nullptr;
    };

    std::map<int, PlayingNote> m_playingNotes;

    // Quantized recording state
    std::unique_ptr<RecordingClock> m_recordingClock;
    INotationPlaybackPtr m_notationPlayback;
    bool m_isQuantizedRecording = false;
    int m_currentRecordingBeat = 0;
    int m_lastQuantizedNoteEndTick = 0; // Track end position for rest insertion

    // Track notes currently being held (waiting for note-off)
    struct PendingNote {
        int pitch;
        int velocity;
        int startTick;
    };
    std::vector<PendingNote> m_pendingNotes;

    // Buffer of completed notes (have both start and end) - written to score when recording stops
    struct RecordedNote {
        int pitch;
        int velocity;
        int startTick;
        int endTick;
    };
    std::vector<RecordedNote> m_recordedNotes;

    // Keyboard chord detection buffer
    QTimer m_keyboardChordTimer;
    struct KeyboardNoteEntry {
        int pitch;
        int velocity;
        int tickTimestamp; // Tick at time key was pressed
    };
    std::vector<KeyboardNoteEntry> m_keyboardNoteBuffer;
    std::set<int> m_heldKeyboardPitches; // Track which pitches are currently held (to filter key repeat)
    static constexpr int KEYBOARD_CHORD_WINDOW_MS = 50; // 50ms window for chord detection

    muse::async::Channel<int> m_currentBeatChanged;
};
}

#endif // MU_NOTATION_NOTATIONMIDIINPUT_H
