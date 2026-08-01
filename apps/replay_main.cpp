// Headless replay runner: drives the g-force-only tilt oscillator from a
// recorded/synthetic acceleration CSV at 200 Hz and dumps both the continuous
// wall height (for plotting/tuning) and the actual LED on/off state (using
// the same geom::LedPanel + sim::ledIsOn logic the firmware will run) to an
// output CSV. Use this against known profiles (e.g. data/brake_0.2g.csv)
// before the SDL2 viewer exists.
//
// Usage: replay_main <input_accel.csv> [output_snapshots.csv] [duration_s]

#include "../geom/ConeGeometry.h"
#include "../geom/LedMapping.h"
#include "../geom/TiltedSurface.h"
#include "../input/ReplayDriver.h"
#include "../sim/LedState.h"
#include "../sim/TiltOscillator.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace {
constexpr double kDt = 1.0 / 200.0;
constexpr int kAzimuthSamples = 24; // continuous wall-height samples, for plotting
} // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: replay_main <input_accel.csv> [output_snapshots.csv] [duration_s]\n";
        return 1;
    }
    const std::string inputPath = argv[1];
    const std::string outputPath = argc >= 3 ? argv[2] : "replay_snapshots.csv";
    const double requestedDuration = argc >= 4 ? std::stod(argv[3]) : -1.0;

    geom::ConeGeometry cone;
    const double fillVolume = cone.defaultFillVolumeMm3(/*freeboardMm=*/8.0);
    const double restHeight = cone.heightForVolume(fillVolume);
    geom::TiltedSurface surface(cone, fillVolume);

    geom::LedPanelConfig ledCfg; // defaults: 24 columns x 12 rows
    geom::LedPanel panel(cone, ledCfg);

    sim::OscillatorParams params;
    params.accelInG = true;
    sim::TiltOscillator osc(surface, params);

    input::ReplayDriver replay(inputPath);
    const double duration = requestedDuration > 0.0 ? requestedDuration : replay.durationSec();

    std::ofstream out(outputPath);
    if (!out.is_open()) {
        std::cerr << "could not open output file " << outputPath << "\n";
        return 1;
    }

    out << "t,tan_phi,tilt_dir_rad,axis_h0,ripple_amp";
    for (int j = 0; j < kAzimuthSamples; ++j) out << ",wall_" << j;
    for (int col = 0; col < panel.columns(); ++col)
        for (int row = 0; row < panel.rows(); ++row) out << ",led_" << col << "_" << row;
    out << "\n";

    const int totalSteps = static_cast<int>(duration / kDt);
    const int snapshotEvery = 4; // 50 Hz snapshots

    std::cout << "cone: rimR=" << cone.dims().rimRadiusMm() << "mm baseR=" << cone.dims().baseRadiusMm()
              << "mm tanAlpha=" << cone.tanAlpha() << "\n";
    std::cout << "fill volume=" << fillVolume / 1000.0 << " ml, rest height=" << restHeight << " mm\n";
    std::cout << "LED panel: " << panel.columns() << " columns x " << panel.rows() << " rows ("
              << panel.ledCount() << " LEDs, serpentine chain)\n";
    std::cout << "running " << totalSteps << " steps (" << duration << " s) from " << inputPath << "\n";

    for (int step = 0; step < totalSteps; ++step) {
        const double t = step * kDt;
        const sim::Vec3 accel = replay.accelAt(t);
        osc.step(accel, kDt);

        if (step % snapshotEvery == 0) {
            out << t << "," << osc.tanPhi() << "," << osc.tiltDirectionRad() << "," << osc.axisHeightMm()
                << "," << osc.rippleAmpMm();
            for (int j = 0; j < kAzimuthSamples; ++j) {
                const double theta = (2.0 * M_PI * j) / kAzimuthSamples;
                out << "," << osc.wallHeightAtAzimuth(theta, t);
            }
            for (int col = 0; col < panel.columns(); ++col)
                for (int row = 0; row < panel.rows(); ++row)
                    out << "," << sim::ledBrightness(osc, panel, col, row, t);
            out << "\n";
        }
    }

    std::cout << "done. wrote snapshots to " << outputPath << "\n";
    return 0;
}
