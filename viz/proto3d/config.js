// TakumiCup prototype config. Plain classic script (not an ES module) on
// purpose: this page is opened directly as a file:// URL with no server,
// and browsers block `fetch()`/`import` of local files under file:// but
// still allow a plain <script src="config.js">, so that's the one loading
// path that works everywhere without needing `python -m http.server`.
//
// Every tunable in the prototype should live here and be read from here --
// nothing should be a bare number sprinkled through index.html.
window.TAKUMI_CONFIG = {
  cup: {
    rimDiameterMm: 89,
    baseDiameterMm: 57,
    heightMm: 110,
    // How full the cup is at rest, as a fraction of heightMm (0..1).
    // Drives where the "on" LEDs sit from rim to bottom at rest, and the
    // volume the h0 lookup table conserves.
    fillFraction: 0.927, // ~8mm freeboard below the rim, matching geom/'s default
  },

  oscillator: {
    naturalFreqScale: 1.0,
    dampingRatio: 0.4,
    referenceLengthMm: 60,
  },

  ripple: {
    gainPerJerk: 0.03,
    decayPerSec: 3.0,
    angularSpeedRadPerSec: 8.0,
    waveNumber: 3,
  },

  // Live sim inputs, not physics tuning -- but their starting values are
  // config too, so the page always starts from a known, reproducible state.
  simDefaults: {
    azG: 1.0,
  },

  led: {
    columns: 24, // azimuthal LED count, matches the eventual hardware panel
    rows: 12,    // vertical LED count per column, matches the eventual hardware panel
    dotRadiusMm: 1.2,
    colorOn: 0x39c6ff,
    colorOff: 0x1a2230,
    // Row placement margin from the absolute base/rim, in mm, so LEDs don't
    // sit exactly on the seams.
    marginFromBaseMm: 6,
    marginFromRimMm: 6,
  },
};
