#include "PluginEditor.h"

ErinSubSynthEditor::ErinSubSynthEditor(ErinSubSynthProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setLookAndFeel(&erinLookAndFeel);
    setSize(700, 400);

    // Section labels
    setupSectionLabel(oscLabel, "OSC");
    setupSectionLabel(filterLabel, "FILTER");
    setupSectionLabel(ampEnvLabel, "AMP ENV");
    setupSectionLabel(filtEnvLabel, "FILT ENV");
    setupSectionLabel(lfoLabel, "LFO");

    // Octave selector
    octaveSelector.addItem("-2", 1);
    octaveSelector.addItem("-1", 2);
    octaveSelector.addItem("0", 3);
    addAndMakeVisible(octaveSelector);
    octaveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.getAPVTS(), "octave", octaveSelector);

    // Oscillator knobs
    setupKnob(osc2Mix, "osc2mix", "MIX");
    setupKnob(osc2Detune, "osc2detune", "DETUNE");

    // Filter knobs
    setupKnob(cutoff, "cutoff", "CUTOFF");
    setupKnob(resonance, "resonance", "RESO");

    // Amp envelope knobs
    setupKnob(ampA, "ampAttack", "A");
    setupKnob(ampD, "ampDecay", "D");
    setupKnob(ampS, "ampSustain", "S");
    setupKnob(ampR, "ampRelease", "R");

    // Filter envelope knobs
    setupKnob(filtA, "filtAttack", "A");
    setupKnob(filtD, "filtDecay", "D");
    setupKnob(filtS, "filtSustain", "S");
    setupKnob(filtR, "filtRelease", "R");
    setupKnob(filtDepth, "filtEnvDepth", "DEPTH");

    // LFO knobs
    setupKnob(lfoRate, "lfoRate", "RATE");
    setupKnob(lfoPitchDepth, "lfoPitchDepth", "PITCH");
    setupKnob(lfoFilterDepth, "lfoFilterDepth", "FILTER");

    // LFO waveform selector
    lfoWaveformSelector.addItem("Sine", 1);
    lfoWaveformSelector.addItem("Triangle", 2);
    lfoWaveformSelector.addItem("Square", 3);
    addAndMakeVisible(lfoWaveformSelector);
    lfoWaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.getAPVTS(), "lfoWaveform", lfoWaveformSelector);

    // Voice mode selector
    voiceModeSelector.addItem("Mono", 1);
    voiceModeSelector.addItem("Poly", 2);
    addAndMakeVisible(voiceModeSelector);
    voiceModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.getAPVTS(), "voiceMode", voiceModeSelector);

    // Voice knobs
    setupKnob(glide, "glide", "GLIDE");

    // Master
    setupKnob(masterVol, "masterVol", "VOLUME");
}

ErinSubSynthEditor::~ErinSubSynthEditor()
{
    setLookAndFeel(nullptr);
}

void ErinSubSynthEditor::setupKnob(KnobWithLabel& kwl, const juce::String& paramId, const juce::String& labelText)
{
    kwl.knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    kwl.knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
    addAndMakeVisible(kwl.knob);

    kwl.label.setText(labelText, juce::dontSendNotification);
    kwl.label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(kwl.label);

    kwl.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), paramId, kwl.knob);
}

void ErinSubSynthEditor::setupSectionLabel(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(14.0f, juce::Font::bold));
    label.setColour(juce::Label::textColourId, ErinLookAndFeel::ledGreen);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void ErinSubSynthEditor::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(ErinLookAndFeel::panelBackground);

    // Horizontal brush texture lines
    g.setColour(juce::Colours::white.withAlpha(0.02f));
    for (int y = 0; y < getHeight(); y += 2)
        g.drawHorizontalLine(y, 0.0f, static_cast<float>(getWidth()));

    // Title bar
    g.setColour(ErinLookAndFeel::panelBackground.darker(0.3f));
    g.fillRect(0, 0, getWidth(), 35);
    g.setColour(ErinLookAndFeel::ledGreen);
    g.setFont(juce::Font(18.0f, juce::Font::bold));
    g.drawText("ERIN SUB SYNTH", 0, 0, getWidth(), 35, juce::Justification::centred);

    // Section divider lines
    int colW = getWidth() / 5;
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    for (int i = 1; i < 5; ++i)
        g.drawVerticalLine(colW * i, 35.0f, static_cast<float>(getHeight() - 50));

    // Bottom bar
    g.setColour(ErinLookAndFeel::panelBackground.darker(0.3f));
    g.fillRect(0, getHeight() - 50, getWidth(), 50);
}

void ErinSubSynthEditor::resized()
{
    int colW = getWidth() / 5;
    int topY = 35;
    int knobSize = 55;
    int labelH = 15;
    int bottomBarY = getHeight() - 50;

    // Section label positions
    oscLabel.setBounds(0, topY, colW, 20);
    filterLabel.setBounds(colW, topY, colW, 20);
    ampEnvLabel.setBounds(colW * 2, topY, colW, 20);
    filtEnvLabel.setBounds(colW * 3, topY, colW, 20);
    lfoLabel.setBounds(colW * 4, topY, colW, 20);

    int contentY = topY + 25;

    // Column 0 (OSC): octaveSelector, osc2Mix knob, osc2Detune knob
    {
        int cx = colW / 2;
        octaveSelector.setBounds(cx - 40, contentY, 80, 22);

        int knobY = contentY + 30;
        osc2Mix.knob.setBounds(cx - knobSize / 2, knobY, knobSize, knobSize);
        osc2Mix.label.setBounds(cx - knobSize / 2, knobY + knobSize, knobSize, labelH);

        knobY += knobSize + labelH + 5;
        osc2Detune.knob.setBounds(cx - knobSize / 2, knobY, knobSize, knobSize);
        osc2Detune.label.setBounds(cx - knobSize / 2, knobY + knobSize, knobSize, labelH);
    }

    // Column 1 (FILTER): cutoff knob, resonance knob
    {
        int cx = colW + colW / 2;

        int knobY = contentY + 10;
        cutoff.knob.setBounds(cx - knobSize / 2, knobY, knobSize, knobSize);
        cutoff.label.setBounds(cx - knobSize / 2, knobY + knobSize, knobSize, labelH);

        knobY += knobSize + labelH + 10;
        resonance.knob.setBounds(cx - knobSize / 2, knobY, knobSize, knobSize);
        resonance.label.setBounds(cx - knobSize / 2, knobY + knobSize, knobSize, labelH);
    }

    // Column 2 (AMP ENV): 2x2 grid of A/D/S/R knobs
    {
        int colX = colW * 2;
        int halfKnob = knobSize / 2;
        int gapX = colW / 2;
        int leftX = colX + gapX / 2 - halfKnob;
        int rightX = colX + gapX + gapX / 2 - halfKnob;

        int row1Y = contentY + 10;
        int row2Y = row1Y + knobSize + labelH + 5;

        ampA.knob.setBounds(leftX, row1Y, knobSize, knobSize);
        ampA.label.setBounds(leftX, row1Y + knobSize, knobSize, labelH);

        ampD.knob.setBounds(rightX, row1Y, knobSize, knobSize);
        ampD.label.setBounds(rightX, row1Y + knobSize, knobSize, labelH);

        ampS.knob.setBounds(leftX, row2Y, knobSize, knobSize);
        ampS.label.setBounds(leftX, row2Y + knobSize, knobSize, labelH);

        ampR.knob.setBounds(rightX, row2Y, knobSize, knobSize);
        ampR.label.setBounds(rightX, row2Y + knobSize, knobSize, labelH);
    }

    // Column 3 (FILT ENV): 2x2 grid of A/D/S/R knobs + depth knob below
    {
        int colX = colW * 3;
        int halfKnob = knobSize / 2;
        int gapX = colW / 2;
        int leftX = colX + gapX / 2 - halfKnob;
        int rightX = colX + gapX + gapX / 2 - halfKnob;

        int row1Y = contentY + 5;
        int row2Y = row1Y + knobSize + labelH + 2;

        filtA.knob.setBounds(leftX, row1Y, knobSize, knobSize);
        filtA.label.setBounds(leftX, row1Y + knobSize, knobSize, labelH);

        filtD.knob.setBounds(rightX, row1Y, knobSize, knobSize);
        filtD.label.setBounds(rightX, row1Y + knobSize, knobSize, labelH);

        filtS.knob.setBounds(leftX, row2Y, knobSize, knobSize);
        filtS.label.setBounds(leftX, row2Y + knobSize, knobSize, labelH);

        filtR.knob.setBounds(rightX, row2Y, knobSize, knobSize);
        filtR.label.setBounds(rightX, row2Y + knobSize, knobSize, labelH);

        int depthY = row2Y + knobSize + labelH + 2;
        int cx = colX + colW / 2;
        filtDepth.knob.setBounds(cx - halfKnob, depthY, knobSize, knobSize);
        filtDepth.label.setBounds(cx - halfKnob, depthY + knobSize, knobSize, labelH);
    }

    // Column 4 (LFO): rate knob, waveform selector, pitch depth and filter depth knobs side by side
    {
        int colX = colW * 4;
        int cx = colX + colW / 2;

        int knobY = contentY + 5;
        lfoRate.knob.setBounds(cx - knobSize / 2, knobY, knobSize, knobSize);
        lfoRate.label.setBounds(cx - knobSize / 2, knobY + knobSize, knobSize, labelH);

        int selectorY = knobY + knobSize + labelH + 5;
        lfoWaveformSelector.setBounds(cx - 45, selectorY, 90, 22);

        int row2Y = selectorY + 28;
        int halfKnob = knobSize / 2;
        int gapX = colW / 2;
        int leftX = colX + gapX / 2 - halfKnob;
        int rightX = colX + gapX + gapX / 2 - halfKnob;

        lfoPitchDepth.knob.setBounds(leftX, row2Y, knobSize, knobSize);
        lfoPitchDepth.label.setBounds(leftX, row2Y + knobSize, knobSize, labelH);

        lfoFilterDepth.knob.setBounds(rightX, row2Y, knobSize, knobSize);
        lfoFilterDepth.label.setBounds(rightX, row2Y + knobSize, knobSize, labelH);
    }

    // Bottom bar: voiceModeSelector, glide knob, masterVol knob (right-aligned)
    {
        int barY = bottomBarY + 10;

        voiceModeSelector.setBounds(10, barY, 80, 25);

        glide.knob.setBounds(100, barY - 5, knobSize, knobSize - 15);
        glide.label.setBounds(100, barY + knobSize - 20, knobSize, labelH);

        int masterX = getWidth() - knobSize - 15;
        masterVol.knob.setBounds(masterX, barY - 5, knobSize, knobSize - 15);
        masterVol.label.setBounds(masterX, barY + knobSize - 20, knobSize, labelH);
    }
}
