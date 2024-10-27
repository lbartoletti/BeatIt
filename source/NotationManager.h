#pragma once

#include "SubdivisionTypes.h"
#include <juce_core/juce_core.h>

/**
 * @class NotationManager
 * @brief Manages musical notation symbols and patterns
 * 
 * Provides access to musical symbols and their combinations for different time signatures.
 * Each pattern is associated with a Subdivision type for audio processing.
 */
class NotationManager 
{
public:
    /**
     * @brief Get the pattern list for a given time signature denominator
     * @param denominator Time signature denominator (1, 2, 4, or 8)
     * @return List of pairs containing the display string and corresponding Subdivision ID
     */
    static std::vector<std::pair<juce::String, int>> getPatternsForDenominator(int denominator);

private:
    /**
     * @brief Get Unicode symbol for whole note
     * @return String containing the symbol
     */
    static const juce::String getWholeNote() 
    { 
        return juce::CharPointer_UTF8("\xF0\x9D\x85\x9C"); 
    }

    /**
     * @brief Get Unicode symbol for half note
     * @return String containing the symbol
     */
    static const juce::String getHalfNote() 
    { 
        return juce::CharPointer_UTF8("\xF0\x9D\x85\x9D"); 
    }

    /**
     * @brief Get Unicode symbol for quarter note
     * @return String containing the symbol
     */
    static const juce::String getQuarterNote() 
    { 
        return juce::String::fromUTF8("\u2669"); 
    }

    /**
     * @brief Get Unicode symbol for eighth note
     * @return String containing the symbol
     */
    static const juce::String getEighthNote() 
    { 
        return juce::String::fromUTF8("\u266A"); 
    }

    /**
     * @brief Get Unicode symbol for beamed eighth notes
     * @return String containing the symbol
     */
    static const juce::String getTwoEighthNotes() 
    { 
        return juce::String::fromUTF8("\u266B"); 
    }

    /**
     * @brief Get Unicode symbol for sixteenth notes
     * @return String containing the symbol
     */
    static const juce::String getSixteenthNotes() 
    { 
        return juce::String::fromUTF8("\u266C"); 
    }
};
