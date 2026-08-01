#include "TiltOscillator.h"

#include <algorithm>
#include <cmath>

namespace sim {

namespace {
constexpr double kG0Ms2 = 9.80665; // 1 g in m/s^2
}

TiltOscillator::TiltOscillator(const geom::TiltedSurface& surface, const OscillatorParams& params)
    : surface_(surface), params_(params) {}

double TiltOscillator::tanPhi() const {
    return std::sqrt(tx_ * tx_ + ty_ * ty_);
}

double TiltOscillator::tiltDirectionRad() const {
    return std::atan2(ty_, tx_);
}

double TiltOscillator::axisHeightMm() const {
    return surface_.h0ForTanPhi(tanPhi());
}

void TiltOscillator::step(const Vec3& accel, double dtSec) {
    const double az = std::max(accel.z, 1e-6); // cup assumed upright; guard divide-by-zero
    const double targetTx = accel.x / az;
    const double targetTy = accel.y / az;

    const double gMagMs2 = (params_.accelInG ? kG0Ms2 : 1.0) *
                            std::sqrt(accel.x * accel.x + accel.y * accel.y + accel.z * accel.z);
    const double lrefM = std::max(params_.referenceLengthMm, 1.0) / 1000.0;
    const double omega0 = params_.naturalFreqScale * std::sqrt(gMagMs2 / lrefM);
    const double dampingCoeff = 2.0 * params_.dampingRatio * omega0;

    const double ax = -omega0 * omega0 * (tx_ - targetTx) - dampingCoeff * vx_;
    const double ay = -omega0 * omega0 * (ty_ - targetTy) - dampingCoeff * vy_;
    vx_ += dtSec * ax;
    vy_ += dtSec * ay;
    tx_ += dtSec * vx_;
    ty_ += dtSec * vy_;

    // Keep the tilt within the geometry lookup's valid range; only clamp
    // position (velocity keeps its value so the spring can still pull back).
    const double mag = std::sqrt(tx_ * tx_ + ty_ * ty_);
    const double maxTanPhi = geom::TiltedSurface::kMaxTanPhi;
    if (mag > maxTanPhi) {
        const double scale = maxTanPhi / mag;
        tx_ *= scale;
        ty_ *= scale;
    }

    // Cosmetic ripple: decay, then add an impulse proportional to how fast
    // the forcing target is changing (not the tilt state itself, so a sharp
    // brake onset/release visibly excites it even before the spring reacts).
    rippleAmp_ *= std::exp(-params_.rippleDecayPerSec * dtSec);
    if (havePrevTarget_ && dtSec > 1e-9) {
        const double dTx = (targetTx - prevTargetTx_) / dtSec;
        const double dTy = (targetTy - prevTargetTy_) / dtSec;
        const double jerk = std::sqrt(dTx * dTx + dTy * dTy);
        rippleAmp_ += params_.rippleGainPerJerk * jerk * dtSec;
    }
    prevTargetTx_ = targetTx;
    prevTargetTy_ = targetTy;
    havePrevTarget_ = true;
}

double TiltOscillator::wallHeightAtAzimuth(double thetaRad, double tSec) const {
    const double phi = tanPhi();
    const double dir = tiltDirectionRad();
    const double h0 = axisHeightMm();

    const double baseZ = surface_.wallHeightAtAzimuth(h0, phi, thetaRad - dir);
    const double rippleZ = rippleAmp_ * std::sin(params_.rippleWaveNumber * thetaRad -
                                                   params_.rippleAngularSpeedRadPerSec * tSec);

    return std::clamp(baseZ + rippleZ, 0.0, surface_.cone().dims().heightMm);
}

} // namespace sim
