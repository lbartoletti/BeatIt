﻿#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "Colors.h"

namespace
{
    // UI Constants
    constexpr int WINDOW_WIDTH = 300;
    constexpr int WINDOW_HEIGHT = 600;
    constexpr int PADDING = 20;
    constexpr float ROTARY_START = juce::MathConstants<float>::pi * 1.2f;
    constexpr float ROTARY_END = juce::MathConstants<float>::pi * 2.8f;
}

//==============================================================================
MetronomeAudioProcessorEditor::MetronomeAudioProcessorEditor (MetronomeAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (WINDOW_WIDTH, WINDOW_HEIGHT);

    // BPM Slider setup
    addAndMakeVisible (bpmSlider);
    bpmSlider.setRange (1.0, 500.0, 1.0);
    bpmSlider.setNumDecimalPlacesToDisplay (0);
    bpmSlider.setSliderStyle (juce::Slider::Rotary);
    bpmSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 25);
    bpmSlider.setColour (juce::Slider::thumbColourId, Colors::cyan);
    bpmSlider.setColour (juce::Slider::rotarySliderFillColourId, Colors::blue);
    bpmSlider.setColour (juce::Slider::rotarySliderOutlineColourId, Colors::grey);
    bpmSlider.setColour (juce::Slider::textBoxTextColourId, Colors::foreground);
    bpmSlider.setColour (juce::Slider::textBoxBackgroundColourId, Colors::backgroundAlt);
    bpmSlider.setColour (juce::Slider::textBoxOutlineColourId, Colors::grey);
    bpmSlider.setTextValueSuffix (" BPM");
    bpmSlider.setRotaryParameters (static_cast<double> (ROTARY_START),
        static_cast<double> (ROTARY_END),
        true);
    bpmSlider.setDoubleClickReturnValue (true, 120.0);
    bpmSlider.setScrollWheelEnabled (true);
    bpmSlider.addListener (this);

    // Buttons setup
    addAndMakeVisible (playButton);
    playButton.setColour (juce::TextButton::buttonColourId, Colors::backgroundAlt);
    playButton.setColour (juce::TextButton::textColourOffId, Colors::foreground);
    playButton.setButtonText (juce::String (juce::CharPointer_UTF8 ("\xE2\x96\xB6")));
    playButton.onClick = [this] { audioProcessor.togglePlayState(); updatePlayButtonText(); };

    addAndMakeVisible (tapTempoButton);
    tapTempoButton.setColour (juce::TextButton::buttonColourId, Colors::backgroundAlt);
    tapTempoButton.setColour (juce::TextButton::textColourOffId, Colors::foreground);
    tapTempoButton.setButtonText ("Tap");
    tapTempoButton.addListener (this);

    // ComboBoxes setup
    auto setupComboBox = [this] (juce::ComboBox& box) {
        addAndMakeVisible (box);
        box.setColour (juce::ComboBox::backgroundColourId, Colors::backgroundAlt);
        box.setColour (juce::ComboBox::textColourId, Colors::foreground);
        box.setColour (juce::ComboBox::outlineColourId, Colors::grey);
    };

    // Beats
    setupComboBox (beatsPerBarComboBox);
    for (int i = 1; i <= 16; ++i)
        beatsPerBarComboBox.addItem (juce::String (i), i);

    setupComboBox (beatDenominatorComboBox);
    beatDenominatorComboBox.addItemList (juce::StringArray ("1", "2", "4", "8"), 1);

    // Click
    setupComboBox (firstBeatSoundComboBox);
    firstBeatSoundComboBox.addItemList (juce::StringArray ("High Click", "Low Click", "Mute"), 1);

    setupComboBox (otherBeatsSoundComboBox);
    otherBeatsSoundComboBox.addItemList (juce::StringArray ("High Click", "Low Click", "Mute"), 1);

    setupComboBox (restSoundComboBox);
    restSoundComboBox.addItemList (juce::StringArray { "Same as Beat", "Rest Sound", "Mute" }, 1);

    restSoundAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        audioProcessor.getState(), "restSound", restSoundComboBox);


    // Subdivision setup
    addAndMakeVisible (subdivisionComboBox);
    subdivisionComboBox.setProcessor (&audioProcessor);
    subdivisionComboBox.updateForDenominator (audioProcessor.getBeatDenominator());


    // Tooltips
    // Add tooltips for beat sound combo boxes
    firstBeatSoundComboBox.setTooltip (
        "Select the sound for the first beat of each bar.\n"
        "High Click: Higher pitched click (1500 Hz)\n"
        "Low Click: Lower pitched click (800 Hz)\n"
        "Mute: No sound");

    otherBeatsSoundComboBox.setTooltip (
        "Select the sound for beats other than the first beat.\n"
        "High Click: Higher pitched click (1500 Hz)\n"
        "Low Click: Lower pitched click (800 Hz)\n"
        "Mute: No sound");

    restSoundComboBox.setTooltip (
        "Select how rests should be played.\n"
        "Same as Beat: Uses the same sound as the current beat\n"
        "Rest Sound: Low frequency sound (200 Hz) to help identify rests\n"
        "Mute: No sound during rests");

    // Add tooltips for other controls
    bpmSlider.setTooltip ("Adjust tempo (1-500 BPM)");

    playButton.setTooltip ("Start/Stop playback");

    tapTempoButton.setTooltip ("Tap repeatedly to set tempo");

    beatsPerBarComboBox.setTooltip ("Set the number of beats per bar (time signature numerator)");

    beatDenominatorComboBox.setTooltip ("Set the beat unit (time signature denominator)");

    subdivisionComboBox.setTooltip (
        "Select the subdivision pattern for each beat.\n"
        "Different patterns available depending on time signature.");

    // Set up attachments
    bpmAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.getState(), "bpm", bpmSlider);

    playAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.getState(), "play", playButton);

    beatsPerBarAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        audioProcessor.getState(), "beatsPerBar", beatsPerBarComboBox);

    beatDenominatorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        audioProcessor.getState(), "beatDenominator", beatDenominatorComboBox);

    firstBeatSoundAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        audioProcessor.getState(), "firstBeatSound", firstBeatSoundComboBox);

    otherBeatsSoundAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        audioProcessor.getState(), "otherBeatsSound", otherBeatsSoundComboBox);

    subdivisionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        audioProcessor.getState(), "subdivision", subdivisionComboBox);

    audioProcessor.getState().addParameterListener ("beatDenominator", this);

    // Set background color
    setColour (juce::DocumentWindow::backgroundColourId, Colors::background);

    startTimer (50); // Update UI 20 times per second
}

MetronomeAudioProcessorEditor::~MetronomeAudioProcessorEditor() {
    audioProcessor.getState().removeParameterListener ("beatDenominator", this);
}

//==============================================================================
void MetronomeAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::DocumentWindow::backgroundColourId));

    if (!beatVisualizers.empty())
    {
        const auto& mutedBeats = audioProcessor.getMutedBeats();
        for (size_t i = 0; i < beatVisualizers.size(); ++i)
        {
            juce::Colour baseColour = (i == 0) ? Colors::red : Colors::blue;

            if (i < mutedBeats.size() && mutedBeats[i])
            {
                baseColour = baseColour.withAlpha (0.3f);
            }

            if (audioProcessor.getPlayState() && i == audioProcessor.getCurrentBeat())
                g.setColour (baseColour.brighter (0.5f));
            else
                g.setColour (baseColour);

            g.fillRect (beatVisualizers[i]);
            g.setColour (Colors::grey);
            g.drawRect (beatVisualizers[i], 1.0f);
        }
    }
}

void MetronomeAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (PADDING);

    // BPM Slider area
    auto bpmArea = area.removeFromTop (static_cast<int> (area.getHeight() * 0.5f));
    bpmSlider.setBounds (bpmArea);

    area.removeFromTop (20); // Spacing

    // Control buttons area
    auto controlArea = area.removeFromTop (40);
    auto controlWidth = (controlArea.getWidth() - 10) / 2;
    playButton.setBounds (controlArea.removeFromLeft (controlWidth));
    controlArea.removeFromLeft (10);
    tapTempoButton.setBounds (controlArea);

    area.removeFromTop (20); // Spacing

    // Time signature area
    auto timeSignatureArea = area.removeFromTop (40);
    beatsPerBarComboBox.setBounds (timeSignatureArea.removeFromLeft (timeSignatureArea.getWidth() / 2).reduced (5));
    beatDenominatorComboBox.setBounds (timeSignatureArea.reduced (5));

    area.removeFromTop (20); // Spacing
    auto subdivisionArea = area.removeFromTop (40);
    subdivisionComboBox.setBounds (subdivisionArea.reduced (5));

    area.removeFromTop (20); // Spacing

    // Sound selection area
    auto soundSelectionArea = area.removeFromTop (30);
    auto comboBoxWidth = (soundSelectionArea.getWidth() - 20) / 3; // Width for each combo box, with 2 spaces of 10px

    // First beat sound combo box
    firstBeatSoundComboBox.setBounds (soundSelectionArea.removeFromLeft (comboBoxWidth));

    // Spacing
    soundSelectionArea.removeFromLeft (10);

    // Other beats sound combo box
    otherBeatsSoundComboBox.setBounds (soundSelectionArea.removeFromLeft (comboBoxWidth));

    // Spacing
    soundSelectionArea.removeFromLeft (10);

    // Rest sound combo box
    restSoundComboBox.setBounds (soundSelectionArea);

    updateBeatVisualizers();
}

void MetronomeAudioProcessorEditor::mouseDown (const juce::MouseEvent& e)
{
    auto localPoint = e.position.toFloat();

    size_t visualizerIndex;
    if (isMouseOverBeatVisualizer (localPoint, visualizerIndex))
    {
        handleBeatVisualizerClick (static_cast<int> (visualizerIndex));
    }
}

bool MetronomeAudioProcessorEditor::isMouseOverBeatVisualizer (
    const juce::Point<float>& position,
    size_t& visualizerIndex) const
{
    for (size_t i = 0; i < beatVisualizers.size(); ++i)
    {
        if (beatVisualizers[i].contains (position))
        {
            visualizerIndex = i;
            return true;
        }
    }
    return false;
}

void MetronomeAudioProcessorEditor::handleBeatVisualizerClick (int beatIndex)
{
    audioProcessor.toggleBeatMute (beatIndex);
    repaint();
}

//==============================================================================
void MetronomeAudioProcessorEditor::timerCallback()
{
    updatePlayButtonText();
    updateBeatVisualizers();
    repaint();
}

void MetronomeAudioProcessorEditor::buttonClicked (juce::Button* button)
{
    if (button == &tapTempoButton)
    {
        if (button == &tapTempoButton)
        {
            audioProcessor.processTapTempo();
        }
    }
}

void MetronomeAudioProcessorEditor::sliderDragStarted (juce::Slider* slider)
{
    if (slider == &bpmSlider)
    {
        // Optional: Handle drag start
    }
}

void MetronomeAudioProcessorEditor::sliderDragEnded (juce::Slider* slider)
{
    if (slider == &bpmSlider)
    {
        // Optional: Handle drag end
    }
}

void MetronomeAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &bpmSlider)
    {
        int newValue = static_cast<int> (std::round (slider->getValue()));
        slider->setValue (newValue, juce::sendNotificationAsync);
    }
}

void MetronomeAudioProcessorEditor::parameterChanged (const juce::String& parameterID, float /*newValue*/)
{
    if (parameterID == "beatDenominator")
    {
        juce::MessageManager::callAsync ([this]() {
            subdivisionComboBox.updateForDenominator (audioProcessor.getBeatDenominator());
        });
    }
}

void MetronomeAudioProcessorEditor::updatePlayButtonText()
{
    if (audioProcessor.getPlayState())
        playButton.setButtonText (juce::String (juce::CharPointer_UTF8 ("\xE2\x96\xA0"))); // Stop symbol
    else
        playButton.setButtonText (juce::String (juce::CharPointer_UTF8 ("\xE2\x96\xB6"))); // Play symbol
}

void MetronomeAudioProcessorEditor::updateBeatVisualizers()
{
    const auto beatsPerBar = audioProcessor.getBeatsPerBar();
    beatVisualizers.clear();
    audioProcessor.updateMutedBeatsSize();

    // Calculate layout for beat visualizers
    const auto totalWidth = static_cast<float> (getWidth() - (2 * PADDING));
    const auto visualizerWidth = (totalWidth / static_cast<float> (beatsPerBar)) - 4.0f;
    const auto height = 20.0f;
    const auto y = static_cast<float> (getHeight()) - height - static_cast<float> (PADDING);
    const auto startX = static_cast<float> (PADDING);

    // Create visualizers
    for (int i = 0; i < beatsPerBar; ++i)
    {
        const auto x = startX + (static_cast<float> (i) * (visualizerWidth + 4.0f));
        beatVisualizers.push_back (juce::Rectangle<float> (x, y, visualizerWidth, height));
    }

    // Update active beat height
    const auto currentBeat = audioProcessor.getCurrentBeat();
    const auto& mutedBeats = audioProcessor.getMutedBeats();
    for (size_t i = 0; i < beatVisualizers.size(); ++i)
    {
        if (audioProcessor.getPlayState() && i == static_cast<size_t> (currentBeat) && (i >= mutedBeats.size() || !mutedBeats[i]))
        {
            beatVisualizers[i].setHeight (30.0f);
        }
        else
        {
            beatVisualizers[i].setHeight (20.0f);
        }
    }
}