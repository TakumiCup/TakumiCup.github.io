#pragma once
// Portable types shared by the physics core. No platform deps: this header
// must compile unchanged on the desktop sim and the ESP32-S3 target.

namespace sim {

struct Vec3 {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

// One accelerometer sample: specific force in the cup-fixed frame (x,y
// horizontal in the cup's local plane, z along the cup axis, rim-ward
// positive). This is g_eff directly -- do not attempt to separate tilt
// from linear acceleration upstream of the sim.
struct AccelSample {
    double tSec = 0.0;
    Vec3 accel; // in g's or m/s^2, consistently with OscillatorParams::accelInG
};

} // namespace sim
