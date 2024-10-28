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
     * @brief Constructor initializes the font and appearance
     */
    // Dans NotesComboBox.h:

    NotesComboBox()
    {
        DBG ("NotesComboBox: Starting font loading...");

        try
        {
            if (BinaryData::Leland_otfSize == 0)
            {
                DBG ("Error: Font data size is 0");
                jassertfalse;
                return;
            }

            DBG ("Loading Leland font...");
            DBG ("Font data size: " << BinaryData::Leland_otfSize);

            auto lelandTypeface = juce::Typeface::createSystemTypefaceFor (
                BinaryData::Leland_otf,
                static_cast<size_t> (BinaryData::Leland_otfSize));

            if (lelandTypeface == nullptr)
            {
                DBG ("Failed to create typeface from binary data");
                jassertfalse;
                return;
            }

            musicFont = juce::Font (lelandTypeface);
            musicFont.setHeight (24.0f);

            DBG ("Leland font created successfully");
        } catch (const std::exception& e)
        {
            DBG ("Exception during font creation: " << e.what());
            jassertfalse;
            return;
        }

        // Configure appearance
        setColour (juce::ComboBox::backgroundColourId, juce::Colours::white);
        setColour (juce::ComboBox::textColourId, juce::Colours::black);
        setColour (juce::ComboBox::outlineColourId, juce::Colours::grey);
    }

    /**
     * @brief Updates the available patterns based on time signature denominator
     * @param denominator The time signature denominator
     */
    void updateForDenominator (int denominator)
    {
        clear();
        auto patterns = NotationManager::getPatternsForDenominator (denominator);

        DBG ("Number of patterns for denominator " << denominator << ": " << patterns.size());

        // Stocker la valeur actuelle avant de vider la combo box
        int currentSubdivision = -1;
        if (processorPtr)
        {
            float normalizedValue = processorPtr->getState().getParameter ("subdivision")->getValue();
            currentSubdivision = static_cast<int> (normalizedValue * (static_cast<float> (static_cast<int> (Subdivision::Count) - 1)));
        }

        // Vérifier si la subdivision actuelle est valide pour le nouveau dénominateur
        bool isCurrentSubdivisionValid = false;
        for (const auto& pattern : patterns)
        {
            if (pattern.second == currentSubdivision)
            {
                isCurrentSubdivisionValid = true;
                break;
            }
        }

        // Ajouter les motifs disponibles
        for (const auto& pattern : patterns)
        {
            addItem (pattern.first, pattern.second + 1);
            DBG ("Adding pattern: " << pattern.first << " with value: " << pattern.second);
        }

        // Restaurer la sélection précédente si elle est valide, sinon sélectionner la première option
        if (isCurrentSubdivisionValid)
        {
            setSelectedId (currentSubdivision + 1, juce::dontSendNotification);
        }
        else
        {
            setSelectedId (1, juce::dontSendNotification);
            if (processorPtr)
            {
                processorPtr->getState().getParameter ("subdivision")->setValueNotifyingHost (0.0f);
            }
        }

        if (processorPtr)
        {
            onChange = [this, patterns]() {
                if (processorPtr)
                {
                    int selectedId = getSelectedId();
                    DBG ("Selected ID: " << selectedId);

                    if (selectedId > 0 && selectedId <= static_cast<int> (patterns.size()))
                    {
                        size_t index = static_cast<size_t> (selectedId - 1);
                        int subdivisionValue = patterns[index].second;
                        DBG ("Setting subdivision value to: " << subdivisionValue);

                        float normalizedValue = static_cast<float> (subdivisionValue) / static_cast<float> (static_cast<int> (Subdivision::Count) - 1);

                        processorPtr->getState().getParameter ("subdivision")->setValueNotifyingHost (normalizedValue);
                    }
                }
            };
        }
    }


    /**
     * @brief Sets the processor reference
     * @param p Pointer to the audio processor
     */
    void setProcessor(MetronomeAudioProcessor* p)
    {
        processorPtr = p;
    }

    /**
     * @brief Custom paint implementation
     */
    void paint(juce::Graphics& g) override
    {
        ComboBox::paint(g);
        
        if (musicFont.getTypefaceName() != "<Sans-Serif>")
        {
            g.setFont(musicFont);
        }
    }

private:
    juce::Font musicFont;
    MetronomeAudioProcessor* processorPtr = nullptr;  // Renamed to avoid shadowing

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NotesComboBox)
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
