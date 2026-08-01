#include "ConeGeometry.h"

#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace geom {

ConeGeometry::ConeGeometry(const CupDimensions& dims)
    : dims_(dims), tanAlpha_(dims.tanAlpha()) {}

double ConeGeometry::radiusAtHeight(double zMm) const {
    const double z = std::clamp(zMm, 0.0, dims_.heightMm);
    return dims_.baseRadiusMm() + z * tanAlpha_;
}

double ConeGeometry::wallHeightAtRadius(double rMm) const {
    if (rMm <= dims_.baseRadiusMm()) return 0.0;
    const double z = (rMm - dims_.baseRadiusMm()) / tanAlpha_;
    return std::clamp(z, 0.0, dims_.heightMm);
}

double ConeGeometry::volumeBelowHeight(double zMm) const {
    const double z = std::clamp(zMm, 0.0, dims_.heightMm);
    const double rb = dims_.baseRadiusMm();
    const double rz = radiusAtHeight(z);
    // Standard frustum volume: (pi*z/3) * (rb^2 + rb*rz + rz^2)
    return (M_PI * z / 3.0) * (rb * rb + rb * rz + rz * rz);
}

double ConeGeometry::heightForVolume(double volumeMm3) const {
    const double vMax = volumeBelowHeight(dims_.heightMm);
    const double target = std::clamp(volumeMm3, 0.0, vMax);
    double lo = 0.0, hi = dims_.heightMm;
    for (int i = 0; i < 60; ++i) {
        const double mid = 0.5 * (lo + hi);
        if (volumeBelowHeight(mid) < target) lo = mid;
        else hi = mid;
    }
    return 0.5 * (lo + hi);
}

double ConeGeometry::defaultFillVolumeMm3(double freeboardMm) const {
    const double fillHeight = std::max(0.0, dims_.heightMm - freeboardMm);
    return volumeBelowHeight(fillHeight);
}

} // namespace geom
