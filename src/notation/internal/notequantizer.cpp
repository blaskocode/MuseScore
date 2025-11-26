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

#include "notequantizer.h"

#include <cmath>
#include <algorithm>

#include "log.h"

using namespace mu::notation;
using namespace mu::engraving;

int NoteQuantizer::quantizePosition(int tick, int ticksPerGrid)
{
    if (ticksPerGrid <= 0) {
        return tick;
    }
    // Round to nearest grid point
    // Since we now buffer notes and quantize after recording stops,
    // there's no latency pressure - we can use proper nearest-neighbor quantization
    int gridIndex = (tick + ticksPerGrid / 2) / ticksPerGrid;
    return gridIndex * ticksPerGrid;
}

Fraction NoteQuantizer::calculateDuration(int startTick, int endTick,
                                          int ticksPerBeat, int ticksPerGrid)
{
    // Safety checks
    if (ticksPerBeat <= 0) {
        ticksPerBeat = 480; // Default to standard MIDI ticks per quarter
    }
    if (ticksPerGrid <= 0) {
        ticksPerGrid = ticksPerBeat / 2; // Default to eighth note
    }

    int rawTicks = endTick - startTick;

    LOGD() << "calculateDuration: startTick=" << startTick << " endTick=" << endTick
           << " rawTicks=" << rawTicks << " ticksPerBeat=" << ticksPerBeat
           << " ticksPerGrid=" << ticksPerGrid;

    // Ensure minimum duration of one grid unit (also handles negative durations)
    if (rawTicks < ticksPerGrid) {
        rawTicks = ticksPerGrid;
    }

    // Convert to fraction (ticks / ticksPerWhole)
    // Standard MIDI uses 480 ticks per quarter note, so 1920 ticks per whole note
    int ticksPerWhole = ticksPerBeat * 4;
    Fraction rawDuration(rawTicks, ticksPerWhole);

    Fraction result = nearestNoteDuration(rawDuration, ticksPerBeat);
    LOGD() << "calculateDuration: rawDuration=" << rawDuration.toString()
           << " result=" << result.toString();

    return result;
}

int NoteQuantizer::gridToTicks(QuantizationGrid grid, int ticksPerBeat)
{
    // ticksPerBeat is typically 480 for a quarter note
    switch (grid) {
    case QuantizationGrid::QUARTER:
        return ticksPerBeat;
    case QuantizationGrid::EIGHTH:
        return ticksPerBeat / 2;
    case QuantizationGrid::SIXTEENTH:
        return ticksPerBeat / 4;
    case QuantizationGrid::THIRTY_SECOND:
        return ticksPerBeat / 8;
    }
    return ticksPerBeat / 2; // Default to eighth
}

Fraction NoteQuantizer::nearestNoteDuration(const Fraction& rawDuration, int ticksPerBeat)
{
    Q_UNUSED(ticksPerBeat)

    const auto& durations = validDurations();

    Fraction nearest = durations[0];
    double minDiff = std::abs(rawDuration.toDouble() - nearest.toDouble());

    for (const auto& dur : durations) {
        double diff = std::abs(rawDuration.toDouble() - dur.toDouble());
        if (diff < minDiff) {
            minDiff = diff;
            nearest = dur;
        }
    }

    return nearest;
}

const std::vector<Fraction>& NoteQuantizer::validDurations()
{
    // Standard note durations plus dotted versions
    // Listed from longest to shortest for efficient searching
    static const std::vector<Fraction> durations = {
        Fraction(1, 1),      // whole
        Fraction(3, 4),      // dotted half
        Fraction(1, 2),      // half
        Fraction(3, 8),      // dotted quarter
        Fraction(1, 4),      // quarter
        Fraction(3, 16),     // dotted eighth
        Fraction(1, 8),      // eighth
        Fraction(3, 32),     // dotted sixteenth
        Fraction(1, 16),     // sixteenth
        Fraction(3, 64),     // dotted 32nd
        Fraction(1, 32),     // 32nd
    };
    return durations;
}
