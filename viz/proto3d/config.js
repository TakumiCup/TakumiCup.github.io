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
    // Drives where the "on" LEDs sit from rim to bottom at rest, the volume
    // the h0 lookup table conserves, and the floor of the spill-warning
    // colour gradient (below this height LEDs show the base colour).
    fillFraction: 0.70,
  },

  // How long (seconds) the top LED row holds solid spill colour after the
  // water surface touches the true rim, regardless of what that row's
  // actual wet/dry state does in the meantime -- a deliberate "there was a
  // breach" indicator, not just instantaneous state.
  rimBreachHoldSec: 2.0,

  // Rendering-only knobs (viz, not physics).
  rendering: {
    // Paper-cup look (opaque white, matte-ish with a slight sheen) rather
    // than the earlier transparent "glass" look -- LEDs are mounted
    // outside the wall surface (see geom/LedMapping's rowRadiusMm), so an
    // opaque cup doesn't hide them.
    cupColor: 0xf2ede0,
    cupOpacity: 0.97,
    cupRoughness: 0.55,
    cupMetalness: 0.05,
    // Vivid, fairly opaque blue rather than a glassy transparent tint, to
    // read as real water seen from above through the open top.
    waterColor: 0x29c2f5,
    waterOpacity: 0.95,
    onLedOpacity: 1.0,
    offLedOpacity: 1.0,
    floorGridOpacity: 1.0,
  },

  oscillator: {
    // Best-guess physical defaults, not just arbitrary numbers: the first
    // sloshing mode of a cylindrical tank has omega1 = sqrt(g*k1*tanh(k1*h))
    // with k1 = 1.8412/R (first zero of J1'), R = rim radius. For this cup
    // (R=44.5mm, h~88mm at 80% fill) that works out to omega1 ~ 20 rad/s
    // (~3.2 Hz) -- naturalFreqScale=1.6 puts our lumped oscillator's omega0
    // (naturalFreqScale * sqrt(|g_eff|/referenceLengthMm)) in that range.
    // dampingRatio=0.1 is a light-damping guess: real water in a smooth
    // open cup settles in roughly 1-2s (a handful of visible oscillations),
    // which at omega0~20 rad/s implies zeta ~ 3/(omega0*t_settle) ~ 0.1.
    naturalFreqScale: 1.6,
    dampingRatio: 0.1,
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
    // Multiplier applied to the simulated joystick's vertical g_eff (not
    // used when driving from a real phone accelerometer, which supplies its
    // own true az): sensitivity=1 behaves as true 1g; higher values make a
    // given joystick deflection produce more tilt, lower values less --
    // labelled "sensitivity" in the UI since that's how it reads to the eye,
    // even though under the hood it's literally 1/az.
    sensitivity: 1.0,

    // Low-pass filter time constant (seconds) applied to the live phone
    // accelerometer signal before it reaches the physics -- raw sensor
    // noise/vibration sits at a much higher frequency than real tilt
    // changes, and our deliberately light damping (oscillator.dampingRatio,
    // chosen for a "watery" overshoot feel) doesn't damp that noise away,
    // so in a moving car it was visibly sloshing from noise alone with no
    // filtering. 0 = unfiltered passthrough; higher = smoother but laggier.
    // Not used for the simulated joystick, only real accelerometer input.
    accelFilterTauSec: 0.15,
  },

  led: {
    columns: 24, // azimuthal LED count, matches the eventual hardware panel
    rows: 12,    // vertical LED count per column, matches the eventual hardware panel
    dotRadiusMm: 1.2,
    // "On" colour is not a single flat colour: rows at or below the fill
    // level (cup.fillFraction) show colorBase; rows above it gradient
    // through colorWarn to colorSpill as row height approaches the topmost
    // LED row, so a row lighting up above the normal fill line reads as an
    // escalating spill warning rather than just "more water".
    colorBase: 0xffffff,
    colorWarn: 0xff8c1a,
    colorSpill: 0xff2020,
    colorOff: 0x1a2230,
    // Row placement margin from the absolute base/rim, in mm, so LEDs don't
    // sit exactly on the seams.
    marginFromBaseMm: 6,
    marginFromRimMm: 6,
  },
};
