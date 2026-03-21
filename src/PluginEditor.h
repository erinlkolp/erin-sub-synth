#pragma once
#include "PluginProcessor.h"
#include "LookAndFeel.h"

class ErinSubSynthEditor : public juce::AudioProcessorEditor
{
public:
    explicit ErinSubSynthEditor(ErinSubSynthProcessor&);
    ~ErinSubSynthEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ErinSubSynthProcessor& processor;
    ErinLookAndFeel erinLookAndFeel;

    struct KnobWithLabel
    {
        juce::Slider knob;
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    // Oscillator section
    juce::ComboBox octaveSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> octaveAttachment;
    KnobWithLabel osc2Mix, osc2Detune;

    // Filter section
    KnobWithLabel cutoff, resonance;

    // Amp envelope
    KnobWithLabel ampA, ampD, ampS, ampR;

    // Filter envelope
    KnobWithLabel filtA, filtD, filtS, filtR, filtDepth;

    // LFO
    KnobWithLabel lfoRate, lfoPitchDepth, lfoFilterDepth;
    juce::ComboBox lfoWaveformSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfoWaveformAttachment;

    // Voice
    juce::ComboBox voiceModeSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> voiceModeAttachment;
    KnobWithLabel glide;

    // Master
    KnobWithLabel masterVol;

    // Section labels
    juce::Label oscLabel, filterLabel, ampEnvLabel, filtEnvLabel, lfoLabel;

    void setupKnob(KnobWithLabel& kwl, const juce::String& paramId, const juce::String& labelText);
    void setupSectionLabel(juce::Label& label, const juce::String& text);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ErinSubSynthEditor)
};
