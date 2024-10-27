#pragma once

#include "BinaryData.h"
#include "PluginProcessor.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * @file PluginEditor.h
 * @brief GUI component implementation for the BeatIt metronome plugin
 * @author Lituus (Loïc Bartoletti)
 * @version 0.0.1
 */
class NotesComboBox : public juce::ComboBox
{
public:
    NotesComboBox()
    {
        DBG ("NotesComboBox: Starting font loading...");

        try
        {
            auto lelandTypeface = juce::Typeface::createSystemTypefaceFor (
                BinaryData::Leland_otf,
                static_cast<size_t> (BinaryData::Leland_otfSize));

            if (lelandTypeface != nullptr)
            {
                DBG ("Loading Leland font...");
                musicFont = juce::Font (lelandTypeface);
                musicFont.setHeight (24.0f);
                DBG ("Leland font created successfully");
                DBG (" - Style: " << lelandTypeface->getStyle());

                // Test différents codes pour la ronde et la blanche
                DBG ("Testing whole and half note codes...");
                // Test different Unicode ranges for whole notes
                juce::String test1 = juce::CharPointer_UTF8 ("\xF0\x9D\x85\x9C"); // U+1D15C
                juce::String test2 = juce::CharPointer_UTF8 ("\xF0\x9D\x85\x9D"); // U+1D15D
                juce::String test3 = juce::CharPointer_UTF8 ("\xF0\x9D\x85\x9E"); // U+1D15E
                DBG ("Test codes created");

                addItem ("Test1 " + test1 + " Whole Note 1", 98);
                addItem ("Test2 " + test2 + " Whole Note 2", 99);
                addItem ("Test3 " + test3 + " Half Note", 100);
            }
            else
            {
                DBG ("Failed to load Leland font");
                return;
            }
        } catch (const std::exception& e)
        {
            DBG ("Exception during font creation: " << e.what());
            return;
        }

        // Symboles de base
        const juce::String quarterNote = juce::String::fromUTF8 ("\u2669"); // Noire (♩)
        const juce::String eighthNote = juce::String::fromUTF8 ("\u266A"); // Croche (♪)
        const juce::String twoEighths = juce::String::fromUTF8 ("\u266B"); // Deux croches liées (♫)
        const juce::String sixteenths = juce::String::fromUTF8 ("\u266C"); // Doubles-croches liées (♬)

        // Soupirs
        const juce::String wholeRest = juce::CharPointer_UTF8 ("\xF0\x9D\x84\xBD"); // Pause
        const juce::String halfRest = juce::CharPointer_UTF8 ("\xF0\x9D\x84\xBE"); // Demi-pause
        const juce::String quarterRest = juce::CharPointer_UTF8 ("\xF0\x9D\x84\xBF"); // Soupir
        const juce::String eighthRest = juce::CharPointer_UTF8 ("\xF0\x9D\x85\x80"); // Demi-soupir
        const juce::String sixteenthRest = juce::CharPointer_UTF8 ("\xF0\x9D\x85\x81"); // Quart de soupir

        int id = 1;

        // Tests alternatifs pour ronde et blanche
        // SMuFL range
        const juce::String wholeNote1 = juce::CharPointer_UTF8 ("\xF0\x9D\x85\x9C"); // U+1D15C MUSICAL SYMBOL WHOLE NOTE
        const juce::String wholeNote2 = juce::CharPointer_UTF8 ("\xF0\x9D\x85\x9D"); // U+1D15D MUSICAL SYMBOL HALF NOTE
        const juce::String wholeNote3 = juce::CharPointer_UTF8 ("\xF0\x9D\x85\x9E"); // U+1D15E MUSICAL SYMBOL QUARTER NOTE

        addItem (wholeNote1 + juce::String (" Whole Note V1"), id++);
        addItem (wholeRest + juce::String (" Whole Rest"), id++);
        addItem (wholeNote2 + juce::String (" Half Note V1"), id++);
        addItem (halfRest + juce::String (" Half Rest"), id++);
        addItem (wholeNote3 + juce::String (" Half Note V2"), id++);
        addItem (halfRest + juce::String (" Half Rest V2"), id++);

        // Autres subdivisions avec les symboles qui fonctionnent
        addItem (quarterNote + juce::String (" 1/4"), id++);
        addItem (quarterRest + juce::String (" Soupir"), id++);
        addItem (eighthNote + juce::String (" 1/8"), id++);
        addItem (eighthRest + juce::String (" Demi-soupir"), id++);
        addItem (sixteenths + juce::String (" 1/16"), id++);
        addItem (sixteenthRest + juce::String (" Quart de soupir"), id++);

        // Configuration de l'apparence
        setColour (juce::ComboBox::textColourId, juce::Colours::black);
        setColour (juce::ComboBox::backgroundColourId, juce::Colours::white);
        setColour (juce::ComboBox::outlineColourId, juce::Colours::grey);

        DBG ("NotesComboBox: Initialization complete with whole/half note tests");
    }

    void paint (juce::Graphics& g) override
    {
        ComboBox::paint (g);

        if (musicFont.getTypefaceName() != "<Sans-Serif>")
        {
            g.setFont (musicFont);

            static bool firstPaint = true;
            if (firstPaint)
            {
                DBG ("Paint with Leland font:");
                DBG (" - Font height: " << g.getCurrentFont().getHeight());
                DBG (" - Font name: " << g.getCurrentFont().getTypefaceName());
                firstPaint = false;
            }
        }
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        ComboBox::mouseDown (e);

        if (e.mods.isCtrlDown() && musicFont.getTypefaceName() != "<Sans-Serif>")
        {
            float currentHeight = musicFont.getHeight();
            if (e.mods.isShiftDown())
                musicFont.setHeight (currentHeight - 2.0f);
            else
                musicFont.setHeight (currentHeight + 2.0f);

            DBG ("Font size adjusted to: " << musicFont.getHeight());
            repaint();
        }
    }

private:
    juce::Font musicFont;
};

/**
 * @class MetronomeAudioProcessorEditor
 * @brief Main editor component for the BeatIt metronome plugin
 * @inherits juce::AudioProcessorEditor
 * @inherits juce::Timer
 * @inherits juce::Button::Listener
 * @inherits juce::Slider::Listener
 * 
 * Provides the graphical user interface with features including:
 * - Interactive BPM control with visual feedback
 * - Play/Stop functionality
 * - Tap tempo capability
 * - Visual beat display with muting options
 * - Time signature configuration
 * - Sound selection for different beat types
 * - State persistence
 */
class MetronomeAudioProcessorEditor : public juce::AudioProcessorEditor,
                                      public juce::Timer,
                                      public juce::Button::Listener,
                                      public juce::Slider::Listener
{
public:
    //==============================================================================
    /** @name Construction and Destruction */
    ///@{

    /**
     * @brief Constructs the editor component
     * @param processor Reference to the audio processor
     */
    MetronomeAudioProcessorEditor (MetronomeAudioProcessor& processor);

    /**
     * @brief Destructor
     */
    ~MetronomeAudioProcessorEditor() override;
    ///@}

    //==============================================================================
    /** @name Component Overrides */
    ///@{

    /**
     * @brief Handles component painting
     * @param g Graphics context used for drawing
     */
    void paint (juce::Graphics& g) override;

    /**
     * @brief Handles component resizing
     */
    void resized() override;
    ///@}

    //==============================================================================
    /** @name Timer Callbacks */
    ///@{

    /**
     * @brief Handles timer callbacks for UI updates
     */
    void timerCallback() override;
    ///@}

    //==============================================================================
    /** @name Control Callbacks */
    ///@{

    /**
     * @brief Handles button click events
     * @param button Pointer to the clicked button
     */
    void buttonClicked (juce::Button* button) override;

    /**
     * @brief Handles start of slider drag
     * @param slider Pointer to the affected slider
     */
    void sliderDragStarted (juce::Slider* slider) override;

    /**
     * @brief Handles end of slider drag
     * @param slider Pointer to the affected slider
     */
    void sliderDragEnded (juce::Slider* slider) override;

    /**
     * @brief Handles slider value changes
     * @param slider Pointer to the changed slider
     */
    void sliderValueChanged (juce::Slider* slider) override;
    ///@}

private:
    //==============================================================================
    /** @name UI Update Methods */
    ///@{

    /**
     * @brief Updates the play button text state
     */
    void updatePlayButtonText();

    /**
     * @brief Updates the beat visualizer states
     */
    void updateBeatVisualizers();

    /**
     * @brief Handles clicks on beat visualizers
     * @param beatIndex Index of the clicked beat
     */
    void handleBeatVisualizerClick (int beatIndex);

    /**
     * @brief Checks if mouse is over a beat visualizer
     * @param position Mouse position to check
     * @param visualizerIndex Output parameter for the found visualizer index
     * @return true if mouse is over a visualizer
     */
    bool isMouseOverBeatVisualizer (const juce::Point<float>& position,
        size_t& visualizerIndex) const;

    /**
     * @brief Handles mouse down events
     * @param e Mouse event details
     */
    void mouseDown (const juce::MouseEvent& e) override;
    ///@}

    //==============================================================================
    /** @name Core References */
    ///@{
    MetronomeAudioProcessor& audioProcessor; /**< Reference to the audio processor */
    ///@}

    //==============================================================================
    /** @name UI Controls */
    ///@{
    juce::Slider bpmSlider; /**< Tempo control slider */
    juce::TextButton playButton; /**< Play/Stop toggle button */
    juce::TextButton tapTempoButton; /**< Tap tempo input button */
    juce::ComboBox beatsPerBarComboBox; /**< Time signature numerator selector */
    juce::ComboBox beatDenominatorComboBox; /**< Time signature denominator selector */
    juce::ComboBox firstBeatSoundComboBox; /**< First beat sound selector */
    juce::ComboBox otherBeatsSoundComboBox; /**< Other beats sound selector */
    NotesComboBox subdivisionComboBox; /**<  Combo box for subdivision pattern selection */

    ///@}

    //==============================================================================
    /** @name Parameter Attachments */
    ///@{
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bpmAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> playAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> beatsPerBarAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> beatDenominatorAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> firstBeatSoundAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> otherBeatsSoundAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> subdivisionAttachment;
    ///@}

    //==============================================================================
    /** @name Visual Components */
    ///@{
    std::vector<juce::Rectangle<float>> beatVisualizers; /**< Beat display rectangles */
    ///@}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MetronomeAudioProcessorEditor)
};