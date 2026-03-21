#include "Voice.h"

void Voice::setSampleRate(double sr) { sampleRate = sr; }

float Voice::midiNoteToFreq(int note) const
{
    return 440.0f * std::pow(2.0f, (note - 69) / 12.0f);
}

void Voice::noteOn(int midiNote, float vel)
{
    bool retrigger = isActive();

    currentNote = midiNote;
    velocity = vel;
    targetFrequency = midiNoteToFreq(midiNote + octaveOffset * 12);
    currentFrequency = targetFrequency;

    if (!retrigger)
    {
        phase1 = 0.0;
        phase2 = 0.0;
        ampLevel = 0.0f;
        filterEnvLevel = 0.0f;
        filterY1 = 0.0f;
        filterY2 = 0.0f;
    }

    ampStage = EnvStage::Attack;
    filterStage = EnvStage::Attack;
}

void Voice::noteOff()
{
    ampStage = EnvStage::Release;
    filterStage = EnvStage::Release;
}

bool Voice::isActive() const
{
    return ampStage != EnvStage::Idle;
}

void Voice::setOctaveOffset(int offset)
{
    if (octaveOffset != offset)
    {
        octaveOffset = offset;
        if (currentNote >= 0)
        {
            targetFrequency = midiNoteToFreq(currentNote + octaveOffset * 12);
            if (glideRate <= 0.0f)
                currentFrequency = targetFrequency;
        }
    }
}
void Voice::setOsc2Mix(float mix) { osc2Mix = mix; }
void Voice::setOsc2Detune(float cents) { osc2DetuneCents = cents; }
void Voice::setFilterCutoff(float hz) { filterCutoff = hz; }
void Voice::setFilterResonance(float res) { filterResonance = res; }
void Voice::setFilterEnvDepth(float depth) { filterEnvDepth = depth; }
void Voice::setGlideRate(float seconds) { glideRate = seconds; }

void Voice::setAmpADSR(float a, float d, float s, float r)
{
    ampAttack = a; ampDecay = d; ampSustain = s; ampRelease = r;
}

void Voice::setFilterADSR(float a, float d, float s, float r)
{
    filtAttack = a; filtDecay = d; filtSustain = s; filtRelease = r;
}

void Voice::setTargetNote(int midiNote, float vel)
{
    targetFrequency = midiNoteToFreq(midiNote + octaveOffset * 12);
    currentNote = midiNote;
    velocity = vel;
}

float Voice::processEnvelope(EnvStage& stage, float& level, float a, float d, float s, float r)
{
    float rate;
    switch (stage)
    {
        case EnvStage::Attack:
            rate = (a > 0.0f) ? 1.0f / (a * (float)sampleRate) : 1.0f;
            level += rate;
            if (level >= 1.0f) { level = 1.0f; stage = EnvStage::Decay; }
            break;
        case EnvStage::Decay:
            rate = (d > 0.0f) ? 1.0f / (d * (float)sampleRate) : 1.0f;
            level -= rate * (1.0f - s);
            if (level <= s) { level = s; stage = EnvStage::Sustain; }
            break;
        case EnvStage::Sustain:
            level = s;
            break;
        case EnvStage::Release:
            rate = (r > 0.0f) ? 1.0f / (r * (float)sampleRate) : 1.0f;
            level -= rate;
            if (level <= 0.0f) { level = 0.0f; stage = EnvStage::Idle; }
            break;
        case EnvStage::Idle:
            level = 0.0f;
            break;
    }
    return level;
}

float Voice::processSample(float lfoFilterMod, float lfoPitchMod)
{
    if (!isActive()) return 0.0f;

    // Glide
    if (glideRate > 0.0f && std::abs(currentFrequency - targetFrequency) > 0.001)
    {
        double glideStep = 1.0 / (glideRate * sampleRate);
        if (currentFrequency < targetFrequency)
            currentFrequency = std::min(currentFrequency * (1.0 + glideStep), targetFrequency);
        else
            currentFrequency = std::max(currentFrequency * (1.0 - glideStep), targetFrequency);
    }

    // Apply LFO pitch modulation (in cents)
    double freq = currentFrequency * std::pow(2.0, lfoPitchMod / 1200.0);

    // Oscillator 1
    double inc1 = freq / sampleRate;
    phase1 += inc1;
    if (phase1 >= 1.0) phase1 -= 1.0;
    float osc1 = static_cast<float>(std::sin(2.0 * juce::MathConstants<double>::pi * phase1));

    // Oscillator 2 (detuned)
    double freq2 = freq * std::pow(2.0, osc2DetuneCents / 1200.0);
    double inc2 = freq2 / sampleRate;
    phase2 += inc2;
    if (phase2 >= 1.0) phase2 -= 1.0;
    float osc2 = static_cast<float>(std::sin(2.0 * juce::MathConstants<double>::pi * phase2));

    // Mix oscillators
    float sample = osc1 * (1.0f - osc2Mix) + osc2 * osc2Mix;

    // Process envelopes
    float ampEnv = processEnvelope(ampStage, ampLevel, ampAttack, ampDecay, ampSustain, ampRelease);
    float filtEnv = processEnvelope(filterStage, filterEnvLevel, filtAttack, filtDecay, filtSustain, filtRelease);

    // Calculate effective filter cutoff
    float envMod = filtEnv * filterEnvDepth * 2000.0f;
    float lfoMod = lfoFilterMod * 2000.0f;
    float effectiveCutoff = std::max(20.0f, std::min(filterCutoff + envMod + lfoMod, 20000.0f));

    // 2-pole resonant low-pass filter (SVF-style)
    float w = std::tan(juce::MathConstants<float>::pi * effectiveCutoff / (float)sampleRate);
    float q = 0.707f + filterResonance * 9.3f;
    float hp = (sample - (1.0f / q + w) * filterY1 - filterY2) / (1.0f + w / q + w * w);
    float bp = w * hp + filterY1;
    float lp = w * bp + filterY2;
    filterY1 = bp;
    filterY2 = lp;

    // Apply amp envelope and velocity
    return lp * ampEnv * velocity;
}
