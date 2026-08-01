#pragma once

#include "../geom/LedMapping.h"
#include "TiltOscillator.h"

#include <cstdint>

namespace sim {

// Fractional brightness (0..1, physically linear) of LED (col, row) right
// now: 1.0 if the water surface at that column's azimuth is at or above the
// row above this one (fully submerged), 0.0 if it hasn't reached this row
// yet, and a linear fraction in between for whichever row the surface is
// currently passing through -- the "sub-LED vertical antialiasing" from the
// original design (fractional brightness on the topmost lit LED), so a
// gently sloshing surface doesn't visibly jump row to row.
double ledBrightness(const TiltOscillator& osc, const geom::LedPanel& panel, int col, int row, double tSec);

// Convenience binary view of the above (brightness > 0), matching the
// panel's earlier on/off-only behaviour where that's all that's needed.
bool ledIsOn(const TiltOscillator& osc, const geom::LedPanel& panel, int col, int row, double tSec);

// Maps a physically-linear brightness (0..1) to a 12-bit PWM duty value
// (0..4095) through a gamma curve, matching how you'd actually drive an
// LED's perceived brightness from a microcontroller's PWM channel -- LEDs
// are roughly linear in radiant power but the eye perceives brightness
// non-linearly, so driving PWM duty directly proportional to physical
// brightness looks wrong (crushed near black); gamma correction fixes that.
uint16_t gammaCorrect12Bit(double brightness01, double gamma = 2.2);

} // namespace sim
