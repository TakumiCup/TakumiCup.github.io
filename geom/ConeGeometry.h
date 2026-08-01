#pragma once
// Exact frustum (paper cup) geometry. No platform deps.
// All lengths in millimetres, volumes in mm^3 unless noted.

namespace geom {

struct CupDimensions {
    double rimDiameterMm = 89.0;
    double baseDiameterMm = 57.0;
    double heightMm = 110.0;

    double rimRadiusMm() const { return rimDiameterMm * 0.5; }
    double baseRadiusMm() const { return baseDiameterMm * 0.5; }
    // Taper half-angle alpha, derived from the two radii and height so it is
    // always self-consistent with the stated dims (~8.3 deg for the defaults).
    double tanAlpha() const { return (rimRadiusMm() - baseRadiusMm()) / heightMm; }
};

// Static (non-tilted) cone geometry queries.
class ConeGeometry {
public:
    explicit ConeGeometry(const CupDimensions& dims = CupDimensions{});

    const CupDimensions& dims() const { return dims_; }
    double tanAlpha() const { return tanAlpha_; }

    // Interior radius of the cup wall at height z above the base (mm).
    // Clamped to [baseRadius, rimRadius] for z outside [0, height].
    double radiusAtHeight(double zMm) const;

    // Inverse of radiusAtHeight: the wall height at which the interior
    // radius equals r (mm). Returns 0 for r <= baseRadius.
    double wallHeightAtRadius(double rMm) const;

    // Volume of liquid filling the frustum from the flat base up to height z
    // (untilted, flat free surface), in mm^3.
    double volumeBelowHeight(double zMm) const;

    // Inverse of volumeBelowHeight via bisection: the flat rest height that
    // holds volume V (mm^3). Volume is monotonic in z so this is safe.
    double heightForVolume(double volumeMm3) const;

    // Default fill volume implied by `freeboardMm` of empty space below the
    // rim, using a flat (untilted) rest surface.
    double defaultFillVolumeMm3(double freeboardMm = 8.0) const;

private:
    CupDimensions dims_;
    double tanAlpha_;
};

} // namespace geom
