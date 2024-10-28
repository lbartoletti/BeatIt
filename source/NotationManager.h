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
    static std::vector<std::pair<juce::String, int>> getPatternsForDenominator (int denominator);

private:
    /**
     * @brief Get Unicode symbol for whole note
     * @return String containing the symbol
     */
    static const juce::String getWholeNote()
    {
        return juce::CharPointer_UTF8 ("\xF0\x9D\x85\x9C");
    }

    /**
     * @brief Get Unicode symbol for half note
     * @return String containing the symbol
     */
    static const juce::String getHalfNote()
    {
        return juce::CharPointer_UTF8 ("\xF0\x9D\x85\x9D");
    }

    /**
     * @brief Get Unicode symbol for quarter note
     * @return String containing the symbol
     */
    static const juce::String getQuarterNote()
    {
        return juce::String::fromUTF8 ("\u2669");
    }

    /**
     * @brief Get Unicode symbol for eighth note
     * @return String containing the symbol
     */
    static const juce::String getEighthNote()
    {
        return juce::String::fromUTF8 ("\u266A");
    }

    /**
     * @brief Get Unicode symbol for beamed eighth notes
     * @return String containing the symbol
     */
    static const juce::String getTwoEighthNotes()
    {
        return juce::String::fromUTF8 ("\u266B");
    }

    /**
     * @brief Get Unicode symbol for sixteenth notes
     * @return String containing the symbol
     */
    static const juce::String getSixteenthNotes()
    {
        return juce::String::fromUTF8 ("\u266C");
    }

    /**
     * @brief Get Unicode symbol for whole rest
     * @return String containing the symbol for a whole note rest
     */
    static const juce::String getWholeRest()
    {
        return juce::CharPointer_UTF8 ("\xF0\x9D\x84\xBD");
    }

    /**
     * @brief Get Unicode symbol for half rest
     * @return String containing the symbol for a half note rest
     */
    static const juce::String getHalfRest()
    {
        return juce::CharPointer_UTF8 ("\xF0\x9D\x84\xBE");
    }

    /**
     * @brief Get Unicode symbol for quarter rest
     * @return String containing the symbol for a quarter note rest
     */
    static const juce::String getQuarterRest()
    {
        return juce::CharPointer_UTF8 ("\xF0\x9D\x84\xBF");
    }

    /**
     * @brief Get Unicode symbol for eighth rest
     * @return String containing the symbol for an eighth note rest
     */
    static const juce::String getEighthRest()
    {
        return juce::CharPointer_UTF8 ("\xF0\x9D\x85\x80");
    }

    /**
     * @brief Get Unicode symbol for sixteenth rest
     * @return String containing the symbol for a sixteenth note rest
     */
    static const juce::String getSixteenthRest()
    {
        return juce::CharPointer_UTF8 ("\xF0\x9D\x85\x81");
    }
};