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

#ifndef MU_NOTATION_NOTEQUANTIZER_H
#define MU_NOTATION_NOTEQUANTIZER_H

#include "engraving/types/fraction.h"
#include "../inotationconfiguration.h"

namespace mu::notation {

class NoteQuantizer
{
public:
    // Quantize a tick position to the nearest grid point
    static int quantizePosition(int tick, int ticksPerGrid);

    // Calculate duration from start/end ticks, round to nearest valid note duration
    static mu::engraving::Fraction calculateDuration(int startTick, int endTick,
                                                      int ticksPerBeat, int ticksPerGrid);

    // Convert QuantizationGrid to ticks
    static int gridToTicks(QuantizationGrid grid, int ticksPerBeat);

    // Find nearest valid note duration (including dotted notes)
    static mu::engraving::Fraction nearestNoteDuration(const mu::engraving::Fraction& rawDuration,
                                                        int ticksPerBeat);

private:
    // Valid note durations (as fractions of a whole note)
    static const std::vector<mu::engraving::Fraction>& validDurations();
};

} // namespace mu::notation

#endif // MU_NOTATION_NOTEQUANTIZER_H
