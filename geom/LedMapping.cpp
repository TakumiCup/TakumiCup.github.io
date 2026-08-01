#include "LedMapping.h"

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace geom {

LedPanel::LedPanel(const ConeGeometry& cone, const LedPanelConfig& config)
    : cone_(cone), config_(config) {}

double LedPanel::columnAzimuthRad(int col) const {
    return (2.0 * M_PI * col) / config_.columns;
}

double LedPanel::rowHeightMm(int row) const {
    const double lo = config_.marginFromBaseMm;
    const double hi = cone_.dims().heightMm - config_.marginFromRimMm;
    if (config_.rows <= 1) return 0.5 * (lo + hi);
    return lo + (hi - lo) * row / (config_.rows - 1);
}

double LedPanel::rowSpacingMm() const {
    if (config_.rows <= 1) return 0.0;
    const double lo = config_.marginFromBaseMm;
    const double hi = cone_.dims().heightMm - config_.marginFromRimMm;
    return (hi - lo) / (config_.rows - 1);
}

double LedPanel::rowRadiusMm(int row) const {
    return cone_.radiusAtHeight(rowHeightMm(row));
}

int LedPanel::serialIndex(int col, int row) const {
    const bool reversed = (col % 2) == 1;
    const int r = reversed ? (config_.rows - 1 - row) : row;
    return col * config_.rows + r;
}

} // namespace geom
