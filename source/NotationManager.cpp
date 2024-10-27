#include "NotationManager.h"

/**
 * @file NotationManager.cpp
 * @brief Implementation of musical notation management utilities
 */

const std::vector<MusicalSymbol>& NotationManager::getAllSymbols() {
    static const std::vector<MusicalSymbol> allSymbols = {
        // Notes
        { "Whole Note", juce::CharPointer_UTF8("\xF0\x9D\x85\x9C"), true },
        { "Half Note", juce::CharPointer_UTF8("\xF0\x9D\x85\x9D"), true },
        { "Quarter Note", juce::String::fromUTF8("\u2669"), true },
        { "Eighth Note", juce::String::fromUTF8("\u266A"), true },
        { "Two Eighth Notes", juce::String::fromUTF8("\u266B"), true },
        { "Sixteenth Notes", juce::String::fromUTF8("\u266C"), true },
        
        // Rests
        { "Whole Rest", juce::CharPointer_UTF8("\xF0\x9D\x84\xBD"), false },
        { "Half Rest", juce::CharPointer_UTF8("\xF0\x9D\x84\xBE"), false },
        { "Quarter Rest", juce::CharPointer_UTF8("\xF0\x9D\x84\xBF"), false },
        { "Eighth Rest", juce::CharPointer_UTF8("\xF0\x9D\x85\x80"), false },
        { "Sixteenth Rest", juce::CharPointer_UTF8("\xF0\x9D\x85\x81"), false }
    };
    return allSymbols;
}

std::vector<NotePattern> NotationManager::getPatternsForDenominator (int denominator)
{
    std::vector<NotePattern> patterns;
    int id = 1;
    const auto& symbols = getAllSymbols();

    switch (denominator)
    {
        case 1: // x/1 patterns
            patterns = {
                { "Whole", symbols[0].symbol, 1, id++, {} }, // Pas de subdivision
                { "Two Half Notes", symbols[1].symbol + symbols[1].symbol, 1, id++, { 0.5f } },
                { "Four Quarter Notes", std::string (4, symbols[2].symbol[0]).c_str(), 1, id++, { 0.25f, 0.5f, 0.75f } }
            };
            break;

        case 2: // x/2 patterns
            patterns = {
                { "Half Note", symbols[1].symbol, 2, id++, {} },
                { "Two Quarter Notes", symbols[2].symbol + symbols[2].symbol, 2, id++, { 0.5f } },
                { "Quarter Rest + Quarter", symbols[8].symbol + symbols[2].symbol, 2, id++, { 0.5f } },
                { "Four Eighth Notes", std::string (2, symbols[4].symbol[0]).c_str(), 2, id++, { 0.25f, 0.5f, 0.75f } }
            };
            break;

        case 4: // x/4 patterns
            patterns = {
                { "Quarter Note", symbols[2].symbol, 4, id++, {} },
                { "Two Eighth Notes", symbols[4].symbol, 4, id++, { 0.5f } },
                { "Three Eighth Notes", symbols[3].symbol + symbols[3].symbol + symbols[3].symbol, 4, id++, { 0.333f, 0.667f } },
                { "Four Sixteenth Notes", symbols[5].symbol, 4, id++, { 0.25f, 0.5f, 0.75f } },
                { "Eighth + Two Sixteenth", symbols[3].symbol + symbols[5].symbol, 4, id++, { 0.5f, 0.75f } },
                { "Two Sixteenth + Eighth", symbols[5].symbol + symbols[3].symbol, 4, id++, { 0.25f, 0.5f } }
            };
            break;

        case 8: // x/8 patterns
            patterns = {
                { "Eighth Note", symbols[3].symbol, 8, id++, {} },
                { "Two Sixteenth Notes", symbols[5].symbol, 8, id++, { 0.5f } },
                { "Eighth Rest + Eighth", symbols[9].symbol + symbols[3].symbol, 8, id++, { 0.5f } },
                { "Three Sixteenth Notes", symbols[5].symbol + symbols[3].symbol + symbols[3].symbol, 8, id++, { 0.333f, 0.667f } },
                { "Four Sixteenth Notes", std::string (2, symbols[5].symbol[0]).c_str(), 8, id++, { 0.25f, 0.5f, 0.75f } }
            };
            break;
    }

    return patterns;
}
