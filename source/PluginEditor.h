#pragma once

#include "BinaryData.h"
#include "NotationManager.h"
#include "PluginProcessor.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * @file PluginEditor.h
 * @brief GUI component implementation for the BeatIt metronome plugin
 * @author Lituus (Loïc Bartoletti)
 * @version 0.0.1
 */

/**
 * @class NotesComboBox
 * @brief Custom ComboBox for displaying musical notation patterns
 * 
 * A specialized ComboBox that displays rhythmic patterns using musical symbols
 * rendered with the Leland font. The patterns shown depend on the current time
 * signature denominator. Supports dynamic font size adjustment and includes
 * debug logging.
 */
class NotesComboBox : public juce::ComboBox
{
public:
    /**
     * @brief Constructor - initializes the font and appearance
     * 
     * Loads the Leland font, sets up the initial font size, and configures
     * the basic appearance of the ComboBox.
     */
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

        // Configuration de l'apparence
        setColour (juce::ComboBox::textColourId, juce::Colours::black);
        setColour (juce::ComboBox::backgroundColourId, juce::Colours::white);
        setColour (juce::ComboBox::outlineColourId, juce::Colours::grey);

        DBG ("NotesComboBox: Initialization complete");

        onChange = [this]() {
            if (auto processor = dynamic_cast<MetronomeAudioProcessor*> (getProcessor()))
            {
                processor->updateSubdivisionFromPattern (getSelectedId());
            }
        };
    }

    /**
     * @brief Updates the available patterns based on time signature denominator
     * 
     * Clears the current items and populates the ComboBox with patterns
     * appropriate for the given time signature denominator.
     * 
     * @param denominator The time signature denominator (1, 2, 4, or 8)
     */
    void updateForDenominator (int denominator)
    {
        clear();
        auto patterns = NotationManager::getPatternsForDenominator (denominator);

        for (const auto& pattern : patterns)
        {
            addItem (pattern.symbols + " " + pattern.name, pattern.id);
            DBG ("Added pattern: " + pattern.name);
        }
    }

    /**
     * @brief Custom paint implementation for the ComboBox
     * 
     * Handles the rendering of the ComboBox with the musical font.
     * Also includes debug logging for the first paint operation.
     * 
     * @param g Graphics context to paint into
     */
    void paint (juce::Graphics& g) override
    {
        ComboBox::paint (g);

        if (musicFont.getTypefaceName() != "<Sans-Serif>")
        {
            g.setFont (musicFont);

            static bool firstPaint = true;
            if (firstPaint)
            {
                DBG ("Paint with custom font:");
                DBG (" - Font height: " << g.getCurrentFont().getHeight());
                DBG (" - Font name: " << g.getCurrentFont().getTypefaceName());
                firstPaint = false;
            }
        }
    }

    /**
     * @brief Handles mouse events for font size adjustment
     * 
     * Implements Ctrl+Click to increase font size and Ctrl+Shift+Click
     * to decrease font size. Updates are logged to debug output.
     * 
     * @param e The mouse event details
     */
    void mouseDown (const juce::MouseEvent& e) override
    {
        ComboBox::mouseDown (e);

        if (e.mods.isCtrlDown() && musicFont.getTypefaceName() != "<Sans-Serif>")
        {
            float currentHeight = musicFont.getHeight();
            if (e.mods.isShiftDown())
            {
                musicFont.setHeight (currentHeight - 2.0f);
                DBG ("Font size decreased to: " << musicFont.getHeight());
            }
            else
            {
                musicFont.setHeight (currentHeight + 2.0f);
                DBG ("Font size increased to: " << musicFont.getHeight());
            }

            repaint();
        }
    }

    /**
     * @brief Gets the current music font
     * @return Reference to the current music font
     */
    const juce::Font& getMusicFont() const { return musicFont; }

    /**
     * @brief Sets a new font size
     * @param newSize The new font size to set
     */
    void setFontSize (float newSize)
    {
        musicFont.setHeight (newSize);
        repaint();
    }
    void setProcessor (MetronomeAudioProcessor* p)
    {
        processor = p;
    }

private:
    MetronomeAudioProcessor* processor = nullptr;

    MetronomeAudioProcessor* getProcessor() const { return processor; }
    juce::Font musicFont; /**< The font used for musical symbols */

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NotesComboBox)
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
                                      public juce::Slider::Listener,
                                      public juce::AudioProcessorValueTreeState::Listener
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

    void updateSubdivisionComboBox (int denominator)
    {
        DBG ("prout" << denominator);
        subdivisionComboBox.updateForDenominator (denominator);
    }

    /**
     * @brief Handles parameter changes from the processor
     * @param parameterID The ID of the changed parameter
     * @param newValue The new value
     */
    void parameterChanged (const juce::String& parameterID, float newValue) override;

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