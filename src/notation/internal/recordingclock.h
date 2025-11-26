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

#ifndef MU_NOTATION_RECORDINGCLOCK_H
#define MU_NOTATION_RECORDINGCLOCK_H

#include <QObject>
#include <QElapsedTimer>
#include <QTimer>

#include "async/asyncable.h"
#include "async/channel.h"
#include "modularity/ioc.h"
#include "../inotationconfiguration.h"

namespace mu::notation {

class RecordingClock : public QObject, public muse::async::Asyncable
{
    Q_OBJECT

    INJECT(INotationConfiguration, configuration)

public:
    explicit RecordingClock(QObject* parent = nullptr);
    ~RecordingClock() override;

    void start(int startTick, double tempoBPS, int ticksPerBeat, int beatsPerMeasure);
    void stop();
    bool isRunning() const;

    // Returns current position in ticks since recording started
    int currentTick() const;

    // Returns elapsed time in milliseconds
    qint64 elapsedMs() const;

    // Accessors for timing parameters
    int ticksPerBeat() const { return m_ticksPerBeat; }

    // Notification channels
    muse::async::Channel<int> beatChanged() const; // Emits beat number (0-based within measure)
    muse::async::Channel<int> metronomeClick() const; // Emits tick position for click

private slots:
    void onTimerTick();

private:
    int msToTicks(qint64 ms) const;
    qint64 ticksToMs(int ticks) const;

    QElapsedTimer m_elapsedTimer;
    QTimer m_metronomeTimer;

    int m_startTick = 0;
    double m_tempoBPS = 2.0; // beats per second (120 BPM default)
    int m_ticksPerBeat = 480;
    int m_beatsPerMeasure = 4;

    int m_currentBeat = 0;
    int m_nextClickTick = 0;

    muse::async::Channel<int> m_beatChanged;
    muse::async::Channel<int> m_metronomeClick;
};

} // namespace mu::notation

#endif // MU_NOTATION_RECORDINGCLOCK_H
