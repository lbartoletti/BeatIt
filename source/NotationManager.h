#pragma once

#include <juce_core/juce_core.h>

/**
 * @file NotationManager.h
 * @brief Musical notation management utilities
 * @author Lituus (Loïc Bartoletti)
 * @version 0.0.1
 */

/**
 * @struct MusicalSymbol
 * @brief Represents a basic musical symbol (note or rest)
 */
struct MusicalSymbol {
    juce::String name;        /**< Display name of the symbol */
    juce::String symbol;      /**< Unicode representation of the symbol */
    bool isNote;             /**< True for notes, false for rests */
};

/**
 * @struct NotePattern
 * @brief Represents a rhythmic pattern with timing information
 */
struct NotePattern
{
    juce::String name; /**< Name of the pattern */
    juce::String symbols; /**< Combined symbols making up the pattern */
    int denominator; /**< Time signature denominator this pattern is valid for */
    int id; /**< Unique identifier for the pattern */
    std::vector<float> timings; /**< Normalized positions (0.0 to 1.0) for each click within the beat */
};

/**
 * @class NotationManager
 * @brief Manages musical notation symbols and patterns
 * 
 * Provides static methods to access musical symbols and generate
 * appropriate rhythm patterns for different time signatures.
 */
class NotationManager {
public:
    /**
     * @brief Gets all available musical symbols
     * @return Vector of all supported musical symbols
     */
    static const std::vector<MusicalSymbol>& getAllSymbols();

    /**
     * @brief Gets patterns valid for a specific time signature denominator
     * @param denominator Time signature denominator (1, 2, 4, or 8)
     * @return Vector of valid patterns for the given denominator
     */
    static std::vector<NotePattern> getPatternsForDenominator(int denominator);
};
