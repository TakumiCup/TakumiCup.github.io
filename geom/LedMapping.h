#pragma once

#include "ConeGeometry.h"

namespace geom {

// Physical LED panel layout: individually-addressable LEDs (e.g. WS2812B)
// mounted directly on the cup's tapered wall, small and thin-wired enough
// that unlit ones are meant to disappear -- dense coverage across the whole
// wall reads as "water", not "a device with lights", rather than a single
// ring at the rim.
//
// This header is the single source of truth for LED placement AND wiring
// order, shared by the ESP32 firmware and any desktop preview. The preview
// must consume this, not reimplement it, so what you see in software is
// what the hardware will actually do.
struct LedPanelConfig {
    int columns = 24; // azimuthal LED count around the wall
    int rows = 12;    // vertical LED count per column

    // Row placement margin from the absolute base/rim, in mm, so the top
    // and bottom rows don't sit exactly on the seams.
    double marginFromBaseMm = 6.0;
    double marginFromRimMm = 6.0;
};

class LedPanel {
public:
    LedPanel(const ConeGeometry& cone, const LedPanelConfig& config);

    const LedPanelConfig& config() const { return config_; }
    int columns() const { return config_.columns; }
    int rows() const { return config_.rows; }
    int ledCount() const { return config_.columns * config_.rows; }

    // Azimuth (radians) of column c, c in [0, columns).
    double columnAzimuthRad(int col) const;

    // Height (mm, from the base) of row r, r in [0, rows), evenly spaced
    // between the configured margins.
    double rowHeightMm(int row) const;

    // Vertical spacing (mm) between adjacent rows. 0 if rows <= 1.
    double rowSpacingMm() const;

    // Wall radius (mm) at row r's height -- where that row's LEDs actually
    // sit on the tapered wall.
    double rowRadiusMm(int row) const;

    // Position of LED (col, row) in the single serial data chain, using a
    // column-major serpentine (boustrophedon) order: up column 0, down
    // column 1, up column 2, ... This is the standard wiring pattern for a
    // single-data-line addressable strip run in a grid -- one continuous
    // cable run, no crossovers.
    int serialIndex(int col, int row) const;

private:
    ConeGeometry cone_;
    LedPanelConfig config_;
};

} // namespace geom
