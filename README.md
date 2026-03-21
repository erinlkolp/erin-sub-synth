# Erin Sub Synth

A sub-oscillator synthesizer VST3 plugin built with C++17 and JUCE 7.0.12.

## Features

- Dual sine oscillators with octave offset (-2, -1, 0) and detune control
- 2-pole resonant low-pass filter (SVF) with envelope and LFO modulation
- Dual ADSR envelopes (amplitude + filter)
- Shared LFO with sine, triangle, and square waveforms
- Mono mode with glide/portamento and poly mode (8 voices)
- Voice stealing (oldest-note priority)
- Skeuomorphic GUI with custom rotary knobs
- Full DAW parameter automation via APVTS

## Building

### Prerequisites

- CMake 3.22+
- C++17 compiler
- **Linux:** `libasound2-dev libfreetype-dev libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxrandr-dev libxrender-dev libwebkit2gtk-4.1-dev libcurl4-openssl-dev`
- **macOS:** Xcode command line tools
- **Windows:** Visual Studio 2019+ or MSVC build tools

### Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j2
```

The VST3 plugin is output to `build/ErinSubSynth_artefacts/Release/VST3/`.

### Install (Linux)

```bash
cp -r "build/ErinSubSynth_artefacts/Release/VST3/Erin Sub Synth.vst3" ~/.vst3/
```

### Tests

```bash
cd build
ctest --output-on-failure -C Release
```

## CI

GitHub Actions builds and tests on Linux, macOS, and Windows on every push/PR to `main`/`master`. Build artifacts (VST3 bundles per platform) are uploaded to each workflow run.

## Project Structure

```
src/
  PluginProcessor.cpp/h  — Audio engine, MIDI handling, parameter tree
  PluginEditor.cpp/h     — GUI layout and controls
  Voice.cpp/h            — Per-voice DSP (oscillators, filter, envelopes)
  LFO.cpp/h              — Shared LFO (sine/triangle/square)
  LookAndFeel.cpp/h      — Custom skeuomorphic UI rendering
test/
  DSPTest.cpp            — Unit tests for oscillator, envelope, LFO, voice allocation
```
