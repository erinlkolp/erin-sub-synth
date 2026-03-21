#include "Voice.h"
#include "LFO.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

static constexpr double kSampleRate = 44100.0;

// ---------------------------------------------------------------------------
// Test 1: Oscillator produces ~440 Hz for MIDI note 69 with octave offset 0
// ---------------------------------------------------------------------------
void testOscillatorFrequency()
{
    Voice v;
    v.setSampleRate(kSampleRate);
    v.setOctaveOffset(0);
    v.setFilterCutoff(20000.0f);
    v.setFilterResonance(0.0f);
    v.setFilterEnvDepth(0.0f);
    v.setAmpADSR(0.001f, 0.0f, 1.0f, 0.1f);   // instant attack, full sustain
    v.setFilterADSR(0.001f, 0.0f, 1.0f, 0.1f);
    v.setOsc2Mix(0.0f);

    v.noteOn(69, 1.0f);  // A4

    int zeroCrossings = 0;
    float prev = 0.0f;
    int totalSamples = static_cast<int>(kSampleRate);  // 1 second

    for (int i = 0; i < totalSamples; ++i)
    {
        float s = v.processSample(0.0f, 0.0f);
        if (i > 0 && ((prev >= 0.0f && s < 0.0f) || (prev < 0.0f && s >= 0.0f)))
            ++zeroCrossings;
        prev = s;
    }

    float estimatedFreq = static_cast<float>(zeroCrossings) / 2.0f;
    assert(std::abs(estimatedFreq - 440.0f) < 5.0f);
    std::cout << "Test: testOscillatorFrequency... PASS" << std::endl;
}

// ---------------------------------------------------------------------------
// Test 2: Octave offset -1 produces ~220 Hz for MIDI note 69
// ---------------------------------------------------------------------------
void testOctaveOffset()
{
    Voice v;
    v.setSampleRate(kSampleRate);
    v.setOctaveOffset(-1);
    v.setFilterCutoff(20000.0f);
    v.setFilterResonance(0.0f);
    v.setFilterEnvDepth(0.0f);
    v.setAmpADSR(0.001f, 0.0f, 1.0f, 0.1f);
    v.setFilterADSR(0.001f, 0.0f, 1.0f, 0.1f);
    v.setOsc2Mix(0.0f);

    v.noteOn(69, 1.0f);  // A4 with octave -1 => A3 = 220 Hz

    int zeroCrossings = 0;
    float prev = 0.0f;
    int totalSamples = static_cast<int>(kSampleRate);

    for (int i = 0; i < totalSamples; ++i)
    {
        float s = v.processSample(0.0f, 0.0f);
        if (i > 0 && ((prev >= 0.0f && s < 0.0f) || (prev < 0.0f && s >= 0.0f)))
            ++zeroCrossings;
        prev = s;
    }

    float estimatedFreq = static_cast<float>(zeroCrossings) / 2.0f;
    assert(std::abs(estimatedFreq - 220.0f) < 5.0f);
    std::cout << "Test: testOctaveOffset... PASS" << std::endl;
}

// ---------------------------------------------------------------------------
// Test 3: Envelope ADSR stages and voice activity
// ---------------------------------------------------------------------------
void testEnvelopeADSR()
{
    Voice v;
    v.setSampleRate(kSampleRate);
    v.setOctaveOffset(0);
    v.setFilterCutoff(20000.0f);
    v.setFilterResonance(0.0f);
    v.setFilterEnvDepth(0.0f);
    // 100ms attack, 100ms decay, 0.5 sustain, 100ms release
    v.setAmpADSR(0.1f, 0.1f, 0.5f, 0.1f);
    v.setFilterADSR(0.1f, 0.1f, 0.5f, 0.1f);
    v.setOsc2Mix(0.0f);

    v.noteOn(60, 1.0f);

    // Voice should be active after noteOn
    assert(v.isActive());

    // Process through attack + decay (200ms = 8820 samples)
    int attackDecaySamples = static_cast<int>(0.2 * kSampleRate);
    for (int i = 0; i < attackDecaySamples; ++i)
        v.processSample(0.0f, 0.0f);

    // Should still be active (in sustain)
    assert(v.isActive());

    // Trigger release
    v.noteOff();

    // Process through release (100ms = 4410 samples) + a small margin
    int releaseSamples = static_cast<int>(0.1 * kSampleRate) + 100;
    for (int i = 0; i < releaseSamples; ++i)
        v.processSample(0.0f, 0.0f);

    // Voice should be inactive after release completes
    assert(!v.isActive());
    std::cout << "Test: testEnvelopeADSR... PASS" << std::endl;
}

// ---------------------------------------------------------------------------
// Test 4: LFO waveforms — sine pitch mod and square first sample
// ---------------------------------------------------------------------------
void testLFOWaveforms()
{
    // Sine: after 1/4 cycle at 1 Hz, pitchMod should be near 100 cents
    {
        LFO lfo;
        lfo.setSampleRate(kSampleRate);
        lfo.setRate(1.0f);
        lfo.setWaveform(LFO::Waveform::Sine);
        lfo.setPitchDepth(100.0f);
        lfo.setFilterDepth(1.0f);
        lfo.reset();

        // Advance 1/4 cycle = sampleRate / 4 samples
        int quarterCycle = static_cast<int>(kSampleRate / 4.0);
        for (int i = 0; i < quarterCycle; ++i)
            lfo.advance();

        float pitchMod = lfo.getPitchMod();
        // At 1/4 cycle, sine(2*pi*0.25) = 1.0, so pitchMod = 100
        assert(std::abs(pitchMod - 100.0f) < 2.0f);
    }

    // Square: first sample should be > 0.9
    {
        LFO lfo;
        lfo.setSampleRate(kSampleRate);
        lfo.setRate(1.0f);
        lfo.setWaveform(LFO::Waveform::Square);
        lfo.setPitchDepth(100.0f);
        lfo.setFilterDepth(1.0f);
        lfo.reset();

        lfo.advance();  // first sample
        float pitchMod = lfo.getPitchMod();
        // Square: phase < 0.5 => currentValue = 1.0, pitchMod = 100
        assert(pitchMod > 0.9f * 100.0f);
    }

    std::cout << "Test: testLFOWaveforms... PASS" << std::endl;
}

// ---------------------------------------------------------------------------
// Test 5: Voice allocation — inactive, then active with correct note, then
//         inactive after noteOff + release
// ---------------------------------------------------------------------------
void testVoiceAllocation()
{
    Voice v;
    v.setSampleRate(kSampleRate);
    v.setOctaveOffset(0);
    v.setFilterCutoff(20000.0f);
    v.setFilterResonance(0.0f);
    v.setFilterEnvDepth(0.0f);
    v.setAmpADSR(0.001f, 0.0f, 1.0f, 0.05f);  // quick attack, short release
    v.setFilterADSR(0.001f, 0.0f, 1.0f, 0.05f);
    v.setOsc2Mix(0.0f);

    // Initially inactive
    assert(!v.isActive());

    // After noteOn, active with correct note
    v.noteOn(64, 0.8f);
    assert(v.isActive());
    assert(v.getCurrentNote() == 64);

    // Process some samples, then noteOff
    for (int i = 0; i < 100; ++i)
        v.processSample(0.0f, 0.0f);

    v.noteOff();

    // Process through release (50ms + margin)
    int releaseSamples = static_cast<int>(0.05 * kSampleRate) + 100;
    for (int i = 0; i < releaseSamples; ++i)
        v.processSample(0.0f, 0.0f);

    assert(!v.isActive());
    std::cout << "Test: testVoiceAllocation... PASS" << std::endl;
}

// ---------------------------------------------------------------------------
int main()
{
    testOscillatorFrequency();
    testOctaveOffset();
    testEnvelopeADSR();
    testLFOWaveforms();
    testVoiceAllocation();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
