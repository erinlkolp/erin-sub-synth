# Erin Sub Synth

C++17 / JUCE 8.0.12 VST3 synthesizer plugin.

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j2
```

Linux deps: `libasound2-dev libfreetype-dev libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxrandr-dev libxrender-dev libwebkit2gtk-4.1-dev libcurl4-openssl-dev`

## Test

```bash
cd build && ctest --output-on-failure -C Release
```

Single test executable `ErinSubSynthTests` covers oscillator frequency, octave offset, ADSR envelope, LFO waveforms, and voice allocation.

## Architecture

- **PluginProcessor** — APVTS parameter tree (21 params), sample-accurate MIDI processing, 8-voice management with mono/poly modes
- **Voice** — Per-voice DSP: dual sine oscillators, SVF low-pass filter, dual ADSR envelopes, glide
- **LFO** — Shared across all voices, modulates pitch (cents) and filter cutoff
- **PluginEditor** — 700x400 skeuomorphic GUI, 5-column layout (OSC, FILTER, AMP ENV, FILT ENV, LFO)
- **LookAndFeel** — Custom rotary knob rendering with 3D gradients

## CI

GitHub Actions workflow at `.github/workflows/ci.yml` builds and tests on Linux, macOS, and Windows. VST3 artifacts are uploaded per platform.

## Conventions

- Limit parallel build jobs to `-j2`
- Install VST3 to `~/.vst3/` on Linux
- JUCE is fetched via CMake FetchContent (no vendored copy)
