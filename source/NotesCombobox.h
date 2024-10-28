#pragma once
/**
 * @class NotesComboBox
 * @brief Custom ComboBox for displaying musical notation patterns
 * 
 * A specialized ComboBox that displays rhythmic patterns using musical symbols
 * rendered with the Leland font. The patterns shown depend on the current time
 * signature denominator. Supports dynamic font size adjustment and includes
 * debug logging.
 */

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "Colors.h"

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

        setColour (juce::ComboBox::backgroundColourId, Colors::backgroundAlt);
        setColour (juce::ComboBox::textColourId, Colors::foreground);
        setColour (juce::ComboBox::outlineColourId, Colors::grey);
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
    void setProcessor (MetronomeAudioProcessor* p)
    {
        processorPtr = p;
    }

    /**
     * @brief Custom paint implementation
     */
    void paint (juce::Graphics& g) override
    {
        ComboBox::paint (g);

        if (musicFont.getTypefaceName() != "<Sans-Serif>")
        {
            g.setFont (musicFont);
        }
    }

private:
    juce::Font musicFont;
    MetronomeAudioProcessor* processorPtr = nullptr; // Renamed to avoid shadowing

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NotesComboBox)
};
