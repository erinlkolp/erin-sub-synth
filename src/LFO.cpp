#include "LFO.h"

static constexpr double pi = 3.14159265358979323846;

void LFO::setSampleRate(double sr)
{
    sampleRate = sr;
}

void LFO::setRate(float hz)
{
    rate = hz;
}

void LFO::setWaveform(Waveform w)
{
    waveform = w;
}

void LFO::setPitchDepth(float cents)
{
    pitchDepth = cents;
}

void LFO::setFilterDepth(float depth)
{
    filterDepth = depth;
}

void LFO::reset()
{
    phase = 0.0;
    currentValue = 0.0f;
}

void LFO::advance()
{
    phase += static_cast<double>(rate) / sampleRate;
    if (phase >= 1.0)
        phase -= 1.0;

    switch (waveform)
    {
        case Waveform::Sine:
            currentValue = static_cast<float>(std::sin(2.0 * pi * phase));
            break;

        case Waveform::Triangle:
            currentValue = static_cast<float>(4.0 * std::abs(phase - 0.5) - 1.0);
            break;

        case Waveform::Square:
            currentValue = (phase < 0.5) ? 1.0f : -1.0f;
            break;
    }
}

float LFO::getPitchMod() const
{
    return currentValue * pitchDepth;
}

float LFO::getFilterMod() const
{
    return currentValue * filterDepth;
}
