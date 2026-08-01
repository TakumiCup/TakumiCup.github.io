#include "TiltedSurface.h"

#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace geom {

namespace {
// Integration resolution for the volume-under-plane numerical integral.
// This only runs at table-build time (64 entries x ~50 bisection steps),
// so it can afford to be reasonably fine without impacting runtime.
constexpr int kIntegR = 64;
constexpr int kIntegTheta = 96;
} // namespace

TiltedSurface::TiltedSurface(const ConeGeometry& cone, double fillVolumeMm3)
    : cone_(cone), fillVolumeMm3_(fillVolumeMm3) {
    buildTable();
}

double TiltedSurface::volumeUnderPlane(double h0, double tanPhi) const {
    const double rRim = cone_.dims().rimRadiusMm();
    const double height = cone_.dims().heightMm;
    const double dr = rRim / kIntegR;
    const double dth = 2.0 * M_PI / kIntegTheta;

    double volume = 0.0;
    for (int i = 0; i < kIntegR; ++i) {
        const double r = (i + 0.5) * dr;
        const double zWall = cone_.wallHeightAtRadius(r);
        for (int j = 0; j < kIntegTheta; ++j) {
            const double theta = (j + 0.5) * dth;
            const double zPlane = h0 + tanPhi * r * std::cos(theta);
            const double top = std::clamp(zPlane, zWall, height);
            const double column = top - zWall; // >= 0
            volume += column * r * dr * dth;
        }
    }
    return volume;
}

void TiltedSurface::buildTable() {
    const double height = cone_.dims().heightMm;
    for (int k = 0; k < kLookupSize; ++k) {
        const double tanPhi = (kMaxTanPhi * k) / (kLookupSize - 1);

        // h0 is monotonic in volume, so bisect. Bounds are generous: the
        // plane's axis height can go well below the floor or above the rim
        // while the tilted cap still holds the target volume off-axis.
        double lo = -height * 2.0;
        double hi = height * 2.0;
        for (int iter = 0; iter < 40; ++iter) {
            const double mid = 0.5 * (lo + hi);
            const double v = volumeUnderPlane(mid, tanPhi);
            if (v < fillVolumeMm3_) lo = mid;
            else hi = mid;
        }
        h0Table_[k] = 0.5 * (lo + hi);
    }
}

double TiltedSurface::h0ForTanPhi(double tanPhi) const {
    const double t = std::clamp(tanPhi, 0.0, kMaxTanPhi);
    const double step = kMaxTanPhi / (kLookupSize - 1);
    const double posF = t / step;
    int idx = static_cast<int>(posF);
    idx = std::clamp(idx, 0, kLookupSize - 2);
    const double frac = posF - idx;
    return h0Table_[idx] * (1.0 - frac) + h0Table_[idx + 1] * frac;
}

double TiltedSurface::wallHeightAtAzimuth(double h0, double tanPhi, double thetaRad) const {
    const double rb = cone_.dims().baseRadiusMm();
    const double tanA = cone_.tanAlpha();
    const double c = std::cos(thetaRad);
    const double denom = 1.0 - tanPhi * tanA * c;
    // Denominator only vanishes for tanPhi*tanA > 1, i.e. tilt steeper than
    // the wall taper itself scaled by tanPhi>~1/tanA (tanA is small, so this
    // is far outside kMaxTanPhi's practical range); guard anyway.
    constexpr double kEps = 1e-6;
    const double safeDenom = std::abs(denom) < kEps ? std::copysign(kEps, denom) : denom;
    const double z = (h0 + rb * tanPhi * c) / safeDenom;
    return std::clamp(z, 0.0, cone_.dims().heightMm);
}

} // namespace geom
