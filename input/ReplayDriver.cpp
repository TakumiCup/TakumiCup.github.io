#include "ReplayDriver.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace input {

namespace {
bool parseRow(const std::string& line, sim::AccelSample& out) {
    std::stringstream ss(line);
    std::string field;
    double vals[4];
    for (int i = 0; i < 4; ++i) {
        if (!std::getline(ss, field, ',')) return false;
        try {
            vals[i] = std::stod(field);
        } catch (...) {
            return false; // non-numeric, e.g. a header row
        }
    }
    out.tSec = vals[0];
    out.accel = sim::Vec3{vals[1], vals[2], vals[3]};
    return true;
}
} // namespace

ReplayDriver::ReplayDriver(const std::string& csvPath) {
    std::ifstream file(csvPath);
    if (!file.is_open()) {
        throw std::runtime_error("ReplayDriver: could not open " + csvPath);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        sim::AccelSample sample;
        if (parseRow(line, sample)) {
            samples_.push_back(sample);
        }
        // Rows that fail to parse (e.g. a "t,ax,ay,az" header) are skipped.
    }

    if (samples_.empty()) {
        throw std::runtime_error("ReplayDriver: no valid samples in " + csvPath);
    }
    std::sort(samples_.begin(), samples_.end(),
              [](const sim::AccelSample& a, const sim::AccelSample& b) { return a.tSec < b.tSec; });
}

double ReplayDriver::durationSec() const {
    return samples_.empty() ? 0.0 : samples_.back().tSec;
}

sim::Vec3 ReplayDriver::accelAt(double tSec) const {
    if (samples_.size() == 1 || tSec <= samples_.front().tSec) return samples_.front().accel;
    if (tSec >= samples_.back().tSec) return samples_.back().accel;

    // Binary search for the bracketing pair.
    auto it = std::lower_bound(samples_.begin(), samples_.end(), tSec,
                                [](const sim::AccelSample& s, double t) { return s.tSec < t; });
    const auto& hi = *it;
    const auto& lo = *(it - 1);
    const double span = hi.tSec - lo.tSec;
    const double frac = span > 1e-12 ? (tSec - lo.tSec) / span : 0.0;

    return sim::Vec3{
        lo.accel.x + (hi.accel.x - lo.accel.x) * frac,
        lo.accel.y + (hi.accel.y - lo.accel.y) * frac,
        lo.accel.z + (hi.accel.z - lo.accel.z) * frac,
    };
}

} // namespace input
