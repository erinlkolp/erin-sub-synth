# Erin Sub Synth — Design Spec

## Overview

A monolithic VST3 sub-oscillator synthesizer instrument plugin built with C++ and JUCE 7+, designed for use in Ardour on Linux. The synth produces deep sub-bass tones using sine wave oscillators with a full-featured control set including dual oscillators, a resonant low-pass filter, dual ADSR envelopes, an LFO, and configurable mono/poly voice modes. The GUI is a skeuomorphic hardware-style panel with custom-drawn knobs and switches.

## Architecture

**Approach:** Monolithic single-file DSP. All DSP lives in a small number of tightly-coupled classes. The GUI is a separate `AudioProcessorEditor` with custom-drawn skeuomorphic components. This keeps complexity low for a focused single-waveform synth.

## DSP Architecture

### Voice Structure

Each voice contains:

- **Sine oscillator (primary)** — phase-accumulator based, anti-aliased. Selectable octave offset: -2, -1, 0 relative to MIDI note.
- **Sine oscillator (secondary)** — detuned against the primary (+/- 50 cents) for thickness. Mix level controllable.
- **Low-pass filter** — 2-pole (12dB/oct) resonant filter with cutoff and resonance controls. Modulated by filter envelope and LFO.
- **Amplitude envelope** — ADSR controlling voice volume.
- **Filter envelope** — separate ADSR controlling filter cutoff modulation with a bipolar depth control.

### Shared Components

- **LFO** — shared across all voices to save CPU. Sine/triangle/square waveform selector. Routes to pitch and/or filter cutoff with independent depth controls for each target.

### Voice Management

- Configurable mono/poly mode via toggle.
- **Mono mode:** last-note priority with glide/portamento (rate knob).
- **Poly mode:** up to 8 voices with voice stealing (oldest note priority).

### Signal Flow

```
MIDI Note -> Oscillator 1 (octave offset) --+
                                             +--> Mix --> LPF --> Amp (* Amp Env) --> Output
             Oscillator 2 (detuned) --------+
                                                    ^
                                              LPF cutoff modulated by: Filter Env + LFO
                                              Pitch modulated by: LFO
```

## Parameters

| Parameter | Range | Default |
|---|---|---|
| **Oscillator** | | |
| Octave Offset | -2, -1, 0 | -1 |
| Osc 2 Mix | 0-100% | 0% (off) |
| Osc 2 Detune | -50 to +50 cents | 0 |
| **Filter** | | |
| Cutoff | 20 Hz - 2 kHz | 500 Hz |
| Resonance | 0-100% | 0% |
| **Amp Envelope** | | |
| Attack | 0-5s | 10ms |
| Decay | 0-5s | 200ms |
| Sustain | 0-100% | 80% |
| Release | 0-5s | 100ms |
| **Filter Envelope** | | |
| Attack | 0-5s | 10ms |
| Decay | 0-5s | 300ms |
| Sustain | 0-100% | 50% |
| Release | 0-5s | 100ms |
| Env Depth | -100% to +100% | 50% |
| **LFO** | | |
| Rate | 0.1-20 Hz | 1 Hz |
| Waveform | Sine / Triangle / Square | Sine |
| Pitch Depth | 0-100 cents | 0 |
| Filter Depth | 0-100% | 0% |
| **Voice** | | |
| Mode | Mono / Poly | Poly |
| Glide | 0-1s | 0 (off) |
| **Master** | | |
| Volume | -inf to +6 dB | 0 dB |

All parameters are exposed to the host as `AudioParameterFloat` / `AudioParameterChoice` for full automation support.

## GUI Design

### Visual Style

Skeuomorphic hardware unit — designed to look like a physical rack or desktop synth module.

### Visual Elements

- Dark metal / brushed aluminum faceplate with subtle texture
- Realistic rotary knobs — raised, shadowed, with indicator lines and printed labels beneath
- Backlit LED-style labels for section headers (OSC, FILTER, AMP, LFO, VOICE)
- Toggle switches for mono/poly mode, LFO waveform, octave selector
- Engraved-style parameter labels — looks stamped/printed on the panel

### Layout

Single-window, fixed-size panel (~700x400px). Sections arranged left to right:

```
+---------------------------------------------------------------------+
|  ERIN SUB SYNTH                                          [logo]     |
+----------+-----------+--------------+--------------+----------------+
|   OSC    |  FILTER   |   AMP ENV    |  FILTER ENV  |     LFO       |
|          |           |              |              |               |
| [OCTAVE] | [CUTOFF]  | [A] [D]      | [A] [D]      | [RATE]        |
|  -2/-1/0 | [RESO]    | [S] [R]      | [S] [R]      | [WAVEFORM]    |
|          |           |              | [DEPTH]      | [PITCH DEP]   |
| [MIX]    |           |              |              | [FILT DEP]    |
| [DETUNE] |           |              |              |               |
+----------+-----------+--------------+--------------+----------------+
|  MODE [MONO|POLY]    [GLIDE]                       [MASTER VOL]    |
+---------------------------------------------------------------------+
```

### Implementation

Custom `juce::LookAndFeel` subclass for skeuomorphic knobs and switches. All components are custom-painted using JUCE's 2D graphics — no external image assets. Keeps the build self-contained.

## Project Structure

```
erin-sub-synth/
|-- CMakeLists.txt
|-- CLAUDE.md
|-- docs/
|   +-- superpowers/specs/
|-- src/
|   |-- PluginProcessor.h / .cpp      -- audio processing, voice management, parameter tree
|   |-- PluginEditor.h / .cpp         -- GUI layout, component wiring
|   |-- Voice.h / .cpp                -- single voice: oscillator, filter, envelopes
|   |-- LookAndFeel.h / .cpp          -- skeuomorphic knob/switch rendering
|   +-- LFO.h / .cpp                  -- shared LFO
+-- test/
    +-- (unit tests for DSP components)
```

## Build

**Build system:** CMake with JUCE fetched via `FetchContent` or git submodule.

**Build target:** VST3 only (via `juce_add_plugin` with `FORMATS VST3`).

**Toolchain:** C++17, GCC or Clang on Linux.

**System dependencies:** `libasound2-dev`, `libfreetype-dev`, `libx11-dev`, `libxrandr-dev`, `libxcursor-dev`, `libxinerama-dev`, `pkg-config`.

**Build commands:**
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
# Copy .vst3 bundle to ~/.vst3/
```

## Testing & Validation

### Unit Tests

- Oscillator: verify correct frequency output, octave offsets, detune accuracy
- Filter: verify cutoff response, resonance behavior
- Envelopes: verify ADSR stages produce expected curves
- Voice allocation: mono last-note priority, poly voice stealing

### Integration Testing (Manual)

- Build the VST3, load in Ardour, verify it appears as an instrument plugin
- Send MIDI notes and confirm audio output
- Verify all parameters are visible and automatable in Ardour
- Test mono/poly switching mid-playback without crashes or glitches

### GUI Testing (Manual)

- All knobs/switches control the correct parameters
- Fixed size — should not resize
- Renders correctly on Linux/X11

Unit tests use JUCE's built-in test runner or a simple `main()` harness.
