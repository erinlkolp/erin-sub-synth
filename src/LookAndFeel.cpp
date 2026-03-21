#include "LookAndFeel.h"

const juce::Colour ErinLookAndFeel::panelBackground{0xFF2A2A2A};
const juce::Colour ErinLookAndFeel::knobBody{0xFF444444};
const juce::Colour ErinLookAndFeel::knobIndicator{0xFFE0E0E0};
const juce::Colour ErinLookAndFeel::ledGreen{0xFF00CC66};
const juce::Colour ErinLookAndFeel::labelText{0xFFCCCCCC};
const juce::Colour ErinLookAndFeel::sectionHeader{0xFF00CC66};

ErinLookAndFeel::ErinLookAndFeel()
{
    setColour(juce::Slider::textBoxTextColourId, labelText);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::Label::textColourId, labelText);
}

void ErinLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y,
    int width, int height, float sliderPos,
    float rotaryStartAngle, float rotaryEndAngle, juce::Slider&)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
    auto radius = std::min(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Knob shadow
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.fillEllipse(centreX - radius + 2, centreY - radius + 2, radius * 2, radius * 2);

    // Knob body — gradient for 3D effect
    juce::ColourGradient bodyGrad(knobBody.brighter(0.3f), centreX, centreY - radius,
                                   knobBody.darker(0.3f), centreX, centreY + radius, false);
    g.setGradientFill(bodyGrad);
    g.fillEllipse(centreX - radius, centreY - radius, radius * 2, radius * 2);

    // Knob edge ring
    g.setColour(knobBody.darker(0.5f));
    g.drawEllipse(centreX - radius, centreY - radius, radius * 2, radius * 2, 1.5f);

    // Indicator line
    juce::Path indicator;
    auto indicatorLength = radius * 0.7f;
    auto lineW = 2.5f;
    indicator.addRectangle(-lineW * 0.5f, -indicatorLength, lineW, indicatorLength);
    indicator.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    g.setColour(knobIndicator);
    g.fillPath(indicator);

    // Center cap
    auto capRadius = radius * 0.2f;
    g.setColour(knobBody.brighter(0.1f));
    g.fillEllipse(centreX - capRadius, centreY - capRadius, capRadius * 2, capRadius * 2);
}

void ErinLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.setColour(label.findColour(juce::Label::textColourId));
    auto font = juce::Font(11.0f);
    g.setFont(font);
    g.drawText(label.getText(), label.getLocalBounds(), juce::Justification::centred, false);
}
