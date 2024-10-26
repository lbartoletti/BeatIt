#pragma once

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
        // Chargement de la fonte musicale
        musicFont = juce::Font (juce::FontOptions()
                                    .withHeight (14.0f)
                                    /* TODO: .withName ("Bravura") */
                                    );

        // Configuration des items avec les symboles musicaux
        addItem (u8"♩ Quarter Note", 1);
        addItem (u8"♫ Two Eighth Notes", 2);
        addItem (u8"♪♪♪ Triplet", 3);
        addItem (u8"♬ Four Sixteenth Notes", 4);
        addItem (u8"♪♬ Eighth + Two Sixteenth", 5);
        addItem (u8"♬♪ Two Sixteenth + Eighth", 6);
    }

    void paint (juce::Graphics& g) override
    {
        ComboBox::paint (g);
        g.setFont (musicFont);
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