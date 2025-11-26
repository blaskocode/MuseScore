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

#include "recordingclock.h"

#include "log.h"

using namespace mu::notation;

RecordingClock::RecordingClock(QObject* parent)
    : QObject(parent)
{
    m_metronomeTimer.setTimerType(Qt::PreciseTimer);
    connect(&m_metronomeTimer, &QTimer::timeout, this, &RecordingClock::onTimerTick);
}

RecordingClock::~RecordingClock()
{
    stop();
}

void RecordingClock::start(int startTick, double tempoBPS, int ticksPerBeat, int beatsPerMeasure)
{
    m_startTick = startTick;
    m_tempoBPS = tempoBPS;
    m_ticksPerBeat = ticksPerBeat;
    m_beatsPerMeasure = beatsPerMeasure;
    m_currentBeat = 0;
    m_nextClickTick = startTick;

    m_elapsedTimer.start();

    // Calculate interval between beats in ms
    int beatIntervalMs = static_cast<int>(1000.0 / m_tempoBPS);
    m_metronomeTimer.start(beatIntervalMs);

    // Trigger first click immediately
    m_metronomeClick.send(m_nextClickTick);
    m_beatChanged.send(m_currentBeat);
}

void RecordingClock::stop()
{
    m_metronomeTimer.stop();
}

bool RecordingClock::isRunning() const
{
    return m_metronomeTimer.isActive();
}

int RecordingClock::currentTick() const
{
    if (!m_elapsedTimer.isValid()) {
        return m_startTick;
    }
    return m_startTick + msToTicks(m_elapsedTimer.elapsed());
}

qint64 RecordingClock::elapsedMs() const
{
    if (!m_elapsedTimer.isValid()) {
        return 0;
    }
    return m_elapsedTimer.elapsed();
}

muse::async::Channel<int> RecordingClock::beatChanged() const
{
    return m_beatChanged;
}

muse::async::Channel<int> RecordingClock::metronomeClick() const
{
    return m_metronomeClick;
}

void RecordingClock::onTimerTick()
{
    m_currentBeat = (m_currentBeat + 1) % m_beatsPerMeasure;
    m_nextClickTick += m_ticksPerBeat;

    m_metronomeClick.send(m_nextClickTick);
    m_beatChanged.send(m_currentBeat);
}

int RecordingClock::msToTicks(qint64 ms) const
{
    // ticks = ms * (ticksPerBeat * beatsPerSecond) / 1000
    return static_cast<int>(ms * m_ticksPerBeat * m_tempoBPS / 1000.0);
}

qint64 RecordingClock::ticksToMs(int ticks) const
{
    // ms = ticks * 1000 / (ticksPerBeat * beatsPerSecond)
    return static_cast<qint64>(ticks * 1000.0 / (m_ticksPerBeat * m_tempoBPS));
}
