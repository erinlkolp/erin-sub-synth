#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>

class Voice
{
public:
    void setSampleRate(double sr);
    void noteOn(int midiNote, float velocity);
    void noteOff();
    bool isActive() const;

    // Parameter setters
    void setOctaveOffset(int offset);       // -2, -1, 0
    void setOsc2Mix(float mix);             // 0.0 - 1.0
    void setOsc2Detune(float cents);        // -50 to +50
    void setFilterCutoff(float hz);         // 20 - 2000
    void setFilterResonance(float res);     // 0.0 - 1.0
    void setAmpADSR(float a, float d, float s, float r);
    void setFilterADSR(float a, float d, float s, float r);
    void setFilterEnvDepth(float depth);    // -1.0 to 1.0
    void setGlideRate(float seconds);

    // Called per-sample from the processor
    float processSample(float lfoFilterMod, float lfoPitchMod);

    int getCurrentNote() const { return currentNote; }
    int getNoteOrder() const { return noteOrder; }
    void setNoteOrder(int order) { noteOrder = order; }
    void setTargetNote(int midiNote, float velocity);  // for glide

private:
    double sampleRate = 44100.0;
    int currentNote = -1;
    int noteOrder = 0;
    float velocity = 0.0f;

    // Oscillator state
    int octaveOffset = -1;
    double phase1 = 0.0;
    double phase2 = 0.0;
    float osc2Mix = 0.0f;
    float osc2DetuneCents = 0.0f;
    double currentFrequency = 0.0;
    double targetFrequency = 0.0;
    float glideRate = 0.0f;

    // Filter state
    float filterCutoff = 500.0f;
    float filterResonance = 0.0f;
    float filterY1 = 0.0f, filterY2 = 0.0f;

    // Amp envelope
    enum class EnvStage { Idle, Attack, Decay, Sustain, Release };
    EnvStage ampStage = EnvStage::Idle;
    float ampLevel = 0.0f;
    float ampAttack = 0.01f, ampDecay = 0.2f, ampSustain = 0.8f, ampRelease = 0.1f;

    // Filter envelope
    EnvStage filterStage = EnvStage::Idle;
    float filterEnvLevel = 0.0f;
    float filtAttack = 0.01f, filtDecay = 0.3f, filtSustain = 0.5f, filtRelease = 0.1f;
    float filterEnvDepth = 0.5f;

    float processEnvelope(EnvStage& stage, float& level, float a, float d, float s, float r);
    float midiNoteToFreq(int note) const;
};
