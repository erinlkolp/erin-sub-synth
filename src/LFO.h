#pragma once
#include <cmath>
#include <juce_audio_basics/juce_audio_basics.h>

class LFO
{
public:
    enum class Waveform { Sine, Triangle, Square };

    void setSampleRate(double sr);
    void setRate(float hz);
    void setWaveform(Waveform w);
    void setPitchDepth(float cents);     // 0-100
    void setFilterDepth(float depth);    // 0.0 - 1.0

    void reset();
    void advance();                      // call once per sample

    float getPitchMod() const;           // returns cents offset
    float getFilterMod() const;          // returns bipolar mod

private:
    double sampleRate = 44100.0;
    float rate = 1.0f;
    Waveform waveform = Waveform::Sine;
    float pitchDepth = 0.0f;
    float filterDepth = 0.0f;
    double phase = 0.0;
    float currentValue = 0.0f;          // -1 to 1
};
