#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class ErinLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ErinLookAndFeel();

    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider&) override;

    void drawLabel(juce::Graphics&, juce::Label&) override;

    // Colors
    static const juce::Colour panelBackground;
    static const juce::Colour knobBody;
    static const juce::Colour knobIndicator;
    static const juce::Colour ledGreen;
    static const juce::Colour labelText;
    static const juce::Colour sectionHeader;
};
