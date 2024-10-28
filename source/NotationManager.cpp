#include "NotationManager.h"

std::vector<std::pair<juce::String, int>> NotationManager::getPatternsForDenominator (int denominator)
{
    std::vector<std::pair<juce::String, int>> patterns;

    switch (denominator)
    {
        case 1: // x/1 patterns
            patterns = {
                { getWholeNote() + " Whole Note",
                    static_cast<int> (Subdivision::NoSubdivision) },
                { getHalfNote() + " " + getHalfNote() + " Two Half Notes",
                    static_cast<int> (Subdivision::Half) },
                { getHalfNote() + " " + getHalfRest() + " Half + Rest",
                    static_cast<int> (Subdivision::HalfAndRest) },
                { getHalfNote() + " " + getHalfNote() + " " + getHalfNote() + " Triplet",
                    static_cast<int> (Subdivision::Triplet) },
                { getQuarterNote() + " " + getQuarterNote() + " " + getQuarterNote() + " " + getQuarterNote() + " Four Quarter Notes",
                    static_cast<int> (Subdivision::Quarter) },
                { getHalfNote() + " " + getQuarterNote() + " " + getQuarterNote() + " Half + Two Quarter Notes",
                    static_cast<int> (Subdivision::HalfQuarter) },
                { getQuarterNote() + " " + getQuarterNote() + " " + getHalfNote() + " Two Quarter + Half Notes",
                    static_cast<int> (Subdivision::QuarterHalf) }
            };
            break;

        case 2: // x/2 patterns
            patterns = {
                { getHalfNote() + " Half Note",
                    static_cast<int> (Subdivision::NoSubdivision) },
                { getQuarterNote() + " " + getQuarterNote() + " Two Quarter Notes",
                    static_cast<int> (Subdivision::Half) },
                { getQuarterNote() + " " + getQuarterRest() + " Quarter + Rest",
                    static_cast<int> (Subdivision::HalfAndRest) },
                { getQuarterNote() + " " + getQuarterNote() + " " + getQuarterNote() + " Triplet",
                    static_cast<int> (Subdivision::Triplet) },
                { getEighthNote() + " " + getEighthNote() + " " + getEighthNote() + " " + getEighthNote() + " Four Eighth Notes",
                    static_cast<int> (Subdivision::Quarter) },
                { getQuarterNote() + " " + getEighthNote() + " " + getEighthNote() + " Quarter + Two Eighth Notes",
                    static_cast<int> (Subdivision::HalfQuarter) },
                { getEighthNote() + " " + getEighthNote() + " " + getQuarterNote() + " Two Eighth + Quarter Notes",
                    static_cast<int> (Subdivision::QuarterHalf) }
            };
            break;

        case 4: // x/4 patterns
            patterns = {
                { getQuarterNote() + " Quarter Note",
                    static_cast<int> (Subdivision::NoSubdivision) },
                { getEighthNote() + " " + getEighthNote() + " Two Eighth Notes",
                    static_cast<int> (Subdivision::Half) },
                { getEighthNote() + " " + getEighthRest() + " Eighth + Rest",
                    static_cast<int> (Subdivision::HalfAndRest) },
                { getEighthNote() + " " + getEighthNote() + " " + getEighthNote() + " Triplet",
                    static_cast<int> (Subdivision::Triplet) },
                { getSixteenthNotes() + " Four Sixteenth Notes",
                    static_cast<int> (Subdivision::Quarter) },
                { getEighthNote() + " " + getSixteenthNotes() + " Eighth + Two Sixteenth",
                    static_cast<int> (Subdivision::HalfQuarter) },
                { getSixteenthNotes() + " " + getEighthNote() + " Two Sixteenth + Eighth",
                    static_cast<int> (Subdivision::QuarterHalf) }
            };
            break;

        case 8: // x/8 patterns
            patterns = {
                { getEighthNote() + " Eighth Note",
                    static_cast<int> (Subdivision::NoSubdivision) },
                { getSixteenthNotes() + " Two Sixteenth Notes",
                    static_cast<int> (Subdivision::Half) },
                { getSixteenthNotes() + " " + getSixteenthRest() + " Sixteenth + Rest",
                    static_cast<int> (Subdivision::HalfAndRest) },
                { getSixteenthNotes() + " " + getSixteenthNotes() + " " + getSixteenthNotes() + " Triplet",
                    static_cast<int> (Subdivision::Triplet) },
                { getSixteenthNotes() + " " + getSixteenthNotes() + " Four 32nd Notes",
                    static_cast<int> (Subdivision::Quarter) },
                { getSixteenthNotes() + " " + getTwoEighthNotes() + " Sixteenth + Two 32nd Notes",
                    static_cast<int> (Subdivision::HalfQuarter) },
                { getTwoEighthNotes() + " " + getSixteenthNotes() + " Two 32nd + Sixteenth Notes",
                    static_cast<int> (Subdivision::QuarterHalf) }
            };
            break;

        default:
            break;
    }

    return patterns;
}