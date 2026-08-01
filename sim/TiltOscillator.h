#pragma once

#include "../geom/TiltedSurface.h"
#include "Types.h"

namespace sim {

// The water surface modeled purely from g_eff: a 2nd-order damped spring
// chasing the tilt that g_eff implies, rendered through the exact cone
// geometry in geom/TiltedSurface. No fluid PDE, no grid, no CFL/stability
// concerns -- this is a lumped single-mode model of "the surface is always
// trying to sit perpendicular to effective gravity, with some inertia and
// damping", plus a purely cosmetic decaying ripple layered on top so it
// doesn't read as a rigid tilting plane.
//
// The tilt is tracked as a 2D vector (tx, ty) = tan(phi) resolved into the
// cup's local x/y axes, rather than an (angle, direction) pair, to avoid
// angle-wraparound bookkeeping -- the underlying dynamics are isotropic so
// this is equivalent and simpler.
struct OscillatorParams {
    // Natural frequency omega0 = naturalFreqScale * sqrt(|g_eff| / referenceLengthMm),
    // i.e. a pendulum-like scaling: the slosh response genuinely speeds up
    // under harder g_eff (hard braking), same physical driver as the wave
    // speed in a real fluid, without needing to simulate the fluid.
    double naturalFreqScale = 1.0;
    double referenceLengthMm = 60.0; // characteristic length (~water depth scale)
    double dampingRatio = 0.4;       // <1 = underdamped (a little overshoot/settle), 1 = critical

    // Cosmetic ripple, NOT fed back into the tilt dynamics: a decaying
    // traveling wave excited by how fast the g_eff-implied target tilt is
    // changing (i.e. by "jerk"), purely to keep the rendered surface from
    // reading as a rigid tilting plane.
    double rippleGainPerJerk = 0.03; // ripple amplitude (mm) injected per unit jerk (1/s of tan-phi)
    double rippleDecayPerSec = 3.0;
    double rippleAngularSpeedRadPerSec = 8.0;
    int rippleWaveNumber = 3; // azimuthal lobes

    bool accelInG = true;
};

class TiltOscillator {
public:
    TiltOscillator(const geom::TiltedSurface& surface, const OscillatorParams& params);

    void step(const Vec3& accel, double dtSec);

    double tanPhi() const;
    double tiltDirectionRad() const; // azimuth the surface climbs toward
    double axisHeightMm() const;     // h0: exact volume-conserving rest height for current tilt
    double rippleAmpMm() const { return rippleAmp_; }

    // Exact wall height at azimuth theta (radians, cup-frame), including the
    // cosmetic ripple. This is what the renderer/LED mapping should sample.
    double wallHeightAtAzimuth(double thetaRad, double tSec) const;

private:
    const geom::TiltedSurface& surface_;
    OscillatorParams params_;

    double tx_ = 0.0, ty_ = 0.0; // tilt vector (tan(phi) components)
    double vx_ = 0.0, vy_ = 0.0; // tilt vector rate of change

    double rippleAmp_ = 0.0;
    double prevTargetTx_ = 0.0, prevTargetTy_ = 0.0;
    bool havePrevTarget_ = false;
};

} // namespace sim
