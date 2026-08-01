#include "LedState.h"

#include <algorithm>
#include <cmath>

namespace sim {

double ledBrightness(const TiltOscillator& osc, const geom::LedPanel& panel, int col, int row, double tSec) {
    const double theta = panel.columnAzimuthRad(col);
    const double waterZ = osc.wallHeightAtAzimuth(theta, tSec);
    const double rowZ = panel.rowHeightMm(row);
    const double spacing = panel.rowSpacingMm();

    if (spacing <= 0.0) return waterZ >= rowZ ? 1.0 : 0.0; // single-row panel: binary fallback

    const double frac = (waterZ - rowZ) / spacing;
    return std::clamp(frac, 0.0, 1.0);
}

bool ledIsOn(const TiltOscillator& osc, const geom::LedPanel& panel, int col, int row, double tSec) {
    return ledBrightness(osc, panel, col, row, tSec) > 0.0;
}

uint16_t gammaCorrect12Bit(double brightness01, double gamma) {
    const double b = std::clamp(brightness01, 0.0, 1.0);
    const double corrected = std::pow(b, 1.0 / gamma);
    return static_cast<uint16_t>(std::lround(corrected * 4095.0));
}

} // namespace sim
