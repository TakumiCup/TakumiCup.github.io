#pragma once

#include "ConeGeometry.h"
#include <array>
#include <vector>

namespace geom {

// Exact rest-state geometry of the free surface when the cup (and hence
// effective gravity direction) is tilted by angle phi from the cup axis.
//
// The free surface is a plane z_plane(r, theta) = h0 + tan(phi) * r * cos(theta),
// where theta=0 is the "high" side (the direction the surface climbs).
// h0 (the plane's height at the cup axis, r=0) has no closed form as a
// function of tan(phi) once volume conservation is imposed against the
// tapered wall, so it is solved numerically and cached in a lookup table.
class TiltedSurface {
public:
    static constexpr int kLookupSize = 64;
    // Table covers tan(phi) in [0, kMaxTanPhi], linearly spaced.
    static constexpr double kMaxTanPhi = 3.0; // ~71.6 degrees, covers hard-brake extremes

    TiltedSurface(const ConeGeometry& cone, double fillVolumeMm3);

    double fillVolumeMm3() const { return fillVolumeMm3_; }

    // Rest height h0 (mm, at cup axis r=0) for a given tan(phi), via linear
    // interpolation of the precomputed 64-entry table.
    double h0ForTanPhi(double tanPhi) const;

    // Exact water surface height at the cup wall as a function of azimuth
    // theta (radians, 0 = high side), given h0 and tan(phi). This is the
    // z(theta) formula used by the renderer for the wall/LED intersection:
    //   z(theta) = (h0 + r_base*tan(phi)*cos(theta)) / (1 - tan(phi)*tan(alpha)*cos(theta))
    double wallHeightAtAzimuth(double h0, double tanPhi, double thetaRad) const;

    // Volume of liquid under the tilted plane (h0, tanPhi) inside the cone,
    // via numerical integration. Exposed mainly for testing/table generation.
    double volumeUnderPlane(double h0, double tanPhi) const;

    const ConeGeometry& cone() const { return cone_; }

private:
    void buildTable();

    ConeGeometry cone_;
    double fillVolumeMm3_;
    std::array<double, kLookupSize> h0Table_{};
};

} // namespace geom
