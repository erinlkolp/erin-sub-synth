#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout ErinSubSynthProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Oscillator
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "octave", "Octave", juce::StringArray{ "-2", "-1", "0" }, 1));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "osc2mix", "Osc 2 Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "osc2detune", "Osc 2 Detune",
        juce::NormalisableRange<float>(-50.0f, 50.0f), 0.0f));

    // Filter
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "cutoff", "Filter Cutoff",
        juce::NormalisableRange<float>(20.0f, 2000.0f, 0.0f, 0.3f), 500.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "resonance", "Resonance",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));

    // Amp Envelope
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "ampAttack", "Amp Attack",
        juce::NormalisableRange<float>(0.0f, 5.0f, 0.0f, 0.3f), 0.01f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "ampDecay", "Amp Decay",
        juce::NormalisableRange<float>(0.0f, 5.0f, 0.0f, 0.3f), 0.2f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "ampSustain", "Amp Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "ampRelease", "Amp Release",
        juce::NormalisableRange<float>(0.0f, 5.0f, 0.0f, 0.3f), 0.1f));

    // Filter Envelope
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filtAttack", "Filter Attack",
        juce::NormalisableRange<float>(0.0f, 5.0f, 0.0f, 0.3f), 0.01f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filtDecay", "Filter Decay",
        juce::NormalisableRange<float>(0.0f, 5.0f, 0.0f, 0.3f), 0.3f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filtSustain", "Filter Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filtRelease", "Filter Release",
        juce::NormalisableRange<float>(0.0f, 5.0f, 0.0f, 0.3f), 0.1f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filtEnvDepth", "Filter Env Depth",
        juce::NormalisableRange<float>(-1.0f, 1.0f), 0.5f));

    // LFO
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "lfoRate", "LFO Rate",
        juce::NormalisableRange<float>(0.1f, 20.0f, 0.0f, 0.3f), 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "lfoWaveform", "LFO Waveform",
        juce::StringArray{ "Sine", "Triangle", "Square" }, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "lfoPitchDepth", "LFO Pitch Depth",
        juce::NormalisableRange<float>(0.0f, 100.0f), 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "lfoFilterDepth", "LFO Filter Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));

    // Voice
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "voiceMode", "Voice Mode",
        juce::StringArray{ "Mono", "Poly" }, 1));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "glide", "Glide",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));

    // Master
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "masterVol", "Master Volume",
        juce::NormalisableRange<float>(-60.0f, 6.0f, 0.0f, 2.0f), 0.0f));

    return { params.begin(), params.end() };
}

//==============================================================================
ErinSubSynthProcessor::ErinSubSynthProcessor()
    : AudioProcessor(BusesProperties()
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

ErinSubSynthProcessor::~ErinSubSynthProcessor() {}

//==============================================================================
void ErinSubSynthProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    for (int i = 0; i < maxVoices; ++i)
        voices[i].setSampleRate(sampleRate);

    lfo.setSampleRate(sampleRate);
    lfo.reset();
}

void ErinSubSynthProcessor::releaseResources() {}

//==============================================================================
void ErinSubSynthProcessor::updateVoiceParameters()
{
    // Octave: index 0=-2, 1=-1, 2=0
    int octaveIndex = static_cast<int>(apvts.getRawParameterValue("octave")->load());
    int octaveOffset = octaveIndex - 2;

    float osc2mix     = apvts.getRawParameterValue("osc2mix")->load();
    float osc2detune  = apvts.getRawParameterValue("osc2detune")->load();
    float cutoff      = apvts.getRawParameterValue("cutoff")->load();
    float resonance   = apvts.getRawParameterValue("resonance")->load();
    float ampA        = apvts.getRawParameterValue("ampAttack")->load();
    float ampD        = apvts.getRawParameterValue("ampDecay")->load();
    float ampS        = apvts.getRawParameterValue("ampSustain")->load();
    float ampR        = apvts.getRawParameterValue("ampRelease")->load();
    float filtA       = apvts.getRawParameterValue("filtAttack")->load();
    float filtD       = apvts.getRawParameterValue("filtDecay")->load();
    float filtS       = apvts.getRawParameterValue("filtSustain")->load();
    float filtR       = apvts.getRawParameterValue("filtRelease")->load();
    float filtDepth   = apvts.getRawParameterValue("filtEnvDepth")->load();
    float glide       = apvts.getRawParameterValue("glide")->load();

    for (int i = 0; i < maxVoices; ++i)
    {
        voices[i].setOctaveOffset(octaveOffset);
        voices[i].setOsc2Mix(osc2mix);
        voices[i].setOsc2Detune(osc2detune);
        voices[i].setFilterCutoff(cutoff);
        voices[i].setFilterResonance(resonance);
        voices[i].setAmpADSR(ampA, ampD, ampS, ampR);
        voices[i].setFilterADSR(filtA, filtD, filtS, filtR);
        voices[i].setFilterEnvDepth(filtDepth);
        voices[i].setGlideRate(glide);
    }

    // LFO
    float lfoRate       = apvts.getRawParameterValue("lfoRate")->load();
    int   lfoWaveIndex  = static_cast<int>(apvts.getRawParameterValue("lfoWaveform")->load());
    float lfoPitchDepth = apvts.getRawParameterValue("lfoPitchDepth")->load();
    float lfoFilterDep  = apvts.getRawParameterValue("lfoFilterDepth")->load();

    lfo.setRate(lfoRate);
    lfo.setWaveform(static_cast<LFO::Waveform>(lfoWaveIndex));
    lfo.setPitchDepth(lfoPitchDepth);
    lfo.setFilterDepth(lfoFilterDep);
}

//==============================================================================
void ErinSubSynthProcessor::handleNoteOn(int midiNote, float vel)
{
    int voiceModeIndex = static_cast<int>(apvts.getRawParameterValue("voiceMode")->load());

    if (voiceModeIndex == 0)  // Mono
    {
        voices[0].setNoteOrder(++noteCounter);
        voices[0].setTargetNote(midiNote, vel);
        if (!voices[0].isActive())
            voices[0].noteOn(midiNote, vel);
    }
    else  // Poly
    {
        // Find a free voice
        int freeIndex = -1;
        for (int i = 0; i < maxVoices; ++i)
        {
            if (!voices[i].isActive())
            {
                freeIndex = i;
                break;
            }
        }

        // If no free voice, steal the oldest (lowest noteOrder)
        if (freeIndex < 0)
        {
            int oldestIndex = 0;
            int oldestOrder = voices[0].getNoteOrder();
            for (int i = 1; i < maxVoices; ++i)
            {
                if (voices[i].getNoteOrder() < oldestOrder)
                {
                    oldestOrder = voices[i].getNoteOrder();
                    oldestIndex = i;
                }
            }
            freeIndex = oldestIndex;
        }

        voices[freeIndex].noteOn(midiNote, vel);
        voices[freeIndex].setNoteOrder(++noteCounter);
    }
}

void ErinSubSynthProcessor::handleNoteOff(int midiNote)
{
    for (int i = 0; i < maxVoices; ++i)
    {
        if (voices[i].getCurrentNote() == midiNote && voices[i].isActive())
            voices[i].noteOff();
    }
}

//==============================================================================
void ErinSubSynthProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    updateVoiceParameters();

    float masterDb = apvts.getRawParameterValue("masterVol")->load();
    float masterGain = juce::Decibels::decibelsToGain(masterDb);

    int numSamples = buffer.getNumSamples();
    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    // Sample-accurate MIDI processing
    int currentSample = 0;

    for (const auto metadata : midiMessages)
    {
        int eventPos = metadata.samplePosition;

        // Render audio up to this MIDI event
        for (; currentSample < eventPos; ++currentSample)
        {
            lfo.advance();
            float pitchMod = lfo.getPitchMod();
            float filterMod = lfo.getFilterMod();

            float sum = 0.0f;
            for (int v = 0; v < maxVoices; ++v)
                sum += voices[v].processSample(filterMod, pitchMod);

            sum *= masterGain;
            sum = juce::jlimit(-1.0f, 1.0f, sum);
            leftChannel[currentSample] = sum;
            if (rightChannel != nullptr)
                rightChannel[currentSample] = sum;
        }

        // Handle MIDI event
        auto message = metadata.getMessage();
        if (message.isNoteOn())
            handleNoteOn(message.getNoteNumber(), message.getFloatVelocity());
        else if (message.isNoteOff())
            handleNoteOff(message.getNoteNumber());
        else if (message.isAllNotesOff() || message.isAllSoundOff())
        {
            for (int v = 0; v < maxVoices; ++v)
                voices[v].noteOff();
        }
    }

    // Render remaining samples after last MIDI event
    for (; currentSample < numSamples; ++currentSample)
    {
        lfo.advance();
        float pitchMod = lfo.getPitchMod();
        float filterMod = lfo.getFilterMod();

        float sum = 0.0f;
        for (int v = 0; v < maxVoices; ++v)
            sum += voices[v].processSample(filterMod, pitchMod);

        sum *= masterGain;
        sum = juce::jlimit(-1.0f, 1.0f, sum);
        leftChannel[currentSample] = sum;
        if (rightChannel != nullptr)
            rightChannel[currentSample] = sum;
    }
}

//==============================================================================
juce::AudioProcessorEditor* ErinSubSynthProcessor::createEditor()
{
    return new ErinSubSynthEditor(*this);
}

//==============================================================================
void ErinSubSynthProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    if (xml != nullptr)
        copyXmlToBinary(*xml, destData);
}

void ErinSubSynthProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ErinSubSynthProcessor();
}
