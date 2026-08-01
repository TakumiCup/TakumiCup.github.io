#pragma once

#include "../sim/Types.h"
#include <string>
#include <vector>

namespace input {

// Plays back a recorded or synthetic acceleration profile from a CSV file
// (columns: t,ax,ay,az -- header row optional, t in seconds, accel in g's).
// Lets you hit the exact same brake/turn profile repeatedly while tuning
// damping and viscosity, independent of the eventual UDP/IMU input path.
class ReplayDriver {
public:
    // Throws std::runtime_error if the file can't be opened or parsed.
    explicit ReplayDriver(const std::string& csvPath);

    // Total duration of the recorded profile, in seconds.
    double durationSec() const;

    // Accel at time t (seconds), linearly interpolated between samples.
    // Clamped to the first/last sample outside the recorded range.
    sim::Vec3 accelAt(double tSec) const;

    size_t sampleCount() const { return samples_.size(); }

private:
    std::vector<sim::AccelSample> samples_;
};

} // namespace input
