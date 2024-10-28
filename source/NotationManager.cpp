#include "NotationManager.h"

std::vector<std::pair<juce::String, int>> NotationManager::getPatternsForDenominator (int denominator)
{
    std::vector<std::pair<juce::String, int>> patterns;

    switch (denominator)
    {
        case 1: // x/1 patterns
            patterns = {
                // 1. Whole note
                { getWholeNote() + " Whole Note",
                    static_cast<int> (Subdivision::NoSubdivision) },

                // 2. Two half notes
                { getHalfNote() + " " + getHalfNote() + " Two Half Notes",
                    static_cast<int> (Subdivision::Half) },

                // 3. Half + Rest
                { getHalfNote() + " " + getHalfRest() + " Half + Rest",
                    static_cast<int> (Subdivision::HalfAndRest) },

                // 4. Rest + Half
                { getHalfRest() + " " + getHalfNote() + " Rest + Half",
                    static_cast<int> (Subdivision::RestHalf) },

                // 5. Triplet
                { getHalfNote() + " " + getHalfNote() + " " + getHalfNote() + " Triplet",
                    static_cast<int> (Subdivision::Triplet) },

                // 6. Rest + Half + Half (triplet)
                { getHalfRest() + " " + getHalfNote() + " " + getHalfNote() + " Rest + Two Half (Triplet)",
                    static_cast<int> (Subdivision::RestHalfHalfTriplet) },

                // 7. Half + Rest + Half (triplet)
                { getHalfNote() + " " + getHalfRest() + " " + getHalfNote() + " Half + Rest + Half (Triplet)",
                    static_cast<int> (Subdivision::HalfRestHalfTriplet) },

                // 8. Half + Half + Rest (triplet)
                { getHalfNote() + " " + getHalfNote() + " " + getHalfRest() + " Two Half + Rest (Triplet)",
                    static_cast<int> (Subdivision::HalfHalfRestTriplet) },

                // 9. Rest + Half + Rest (triplet)
                { getHalfRest() + " " + getHalfNote() + " " + getHalfRest() + " Rest + Half + Rest (Triplet)",
                    static_cast<int> (Subdivision::RestHalfRestTriplet) },

                // 10. Four Quarter Notes
                { getQuarterNote() + " " + getQuarterNote() + " " + getQuarterNote() + " " + getQuarterNote() + " Four Quarter Notes",
                    static_cast<int> (Subdivision::Quarter) },

                // 11. Rest + Quarter + Rest + Quarter
                { getQuarterRest() + " " + getQuarterNote() + " " + getQuarterRest() + " " + getQuarterNote() + " Rest + Quarter + Rest + Quarter",
                    static_cast<int> (Subdivision::RestEighthPattern) },

                // 12. Quarter + Quarter + Half
                { getQuarterNote() + " " + getQuarterNote() + " " + getHalfNote() + " Two Quarter + Half",
                    static_cast<int> (Subdivision::EighthEighthQuarter) },

                // 13. Half + Quarter + Quarter
                { getHalfNote() + " " + getQuarterNote() + " " + getQuarterNote() + " Half + Two Quarter",
                    static_cast<int> (Subdivision::QuarterEighthEighth) },

                // 14. Quarter + Half + Quarter
                { getQuarterNote() + " " + getHalfNote() + " " + getQuarterNote() + " Quarter + Half + Quarter",
                    static_cast<int> (Subdivision::EighthQuarterEighth) }
            };
            break;

        case 2: // x/2 patterns
            patterns = {
                // 1. Half note
                { getHalfNote() + " Half Note",
                    static_cast<int> (Subdivision::NoSubdivision) },

                // 2. Two quarter notes
                { getQuarterNote() + " " + getQuarterNote() + " Two Quarter Notes",
                    static_cast<int> (Subdivision::Half) },

                // 3. Quarter + Rest
                { getQuarterNote() + " " + getQuarterRest() + " Quarter + Rest",
                    static_cast<int> (Subdivision::HalfAndRest) },

                // 4. Rest + Quarter
                { getQuarterRest() + " " + getQuarterNote() + " Rest + Quarter",
                    static_cast<int> (Subdivision::RestHalf) },

                // 5. Triplet
                { getQuarterNote() + " " + getQuarterNote() + " " + getQuarterNote() + " Triplet",
                    static_cast<int> (Subdivision::Triplet) },

                // 6. Rest + Quarter + Quarter (triplet)
                { getQuarterRest() + " " + getQuarterNote() + " " + getQuarterNote() + " Rest + Two Quarter (Triplet)",
                    static_cast<int> (Subdivision::RestHalfHalfTriplet) },

                // 7. Quarter + Rest + Quarter (triplet)
                { getQuarterNote() + " " + getQuarterRest() + " " + getQuarterNote() + " Quarter + Rest + Quarter (Triplet)",
                    static_cast<int> (Subdivision::HalfRestHalfTriplet) },

                // 8. Quarter + Quarter + Rest (triplet)
                { getQuarterNote() + " " + getQuarterNote() + " " + getQuarterRest() + " Two Quarter + Rest (Triplet)",
                    static_cast<int> (Subdivision::HalfHalfRestTriplet) },

                // 9. Rest + Quarter + Rest (triplet)
                { getQuarterRest() + " " + getQuarterNote() + " " + getQuarterRest() + " Rest + Quarter + Rest (Triplet)",
                    static_cast<int> (Subdivision::RestHalfRestTriplet) },

                // 10. Four Eighth Notes
                { getEighthNote() + " " + getEighthNote() + " " + getEighthNote() + " " + getEighthNote() + " Four Eighth Notes",
                    static_cast<int> (Subdivision::Quarter) },

                // 11. Rest + Eight + Rest + Eight
                { getEighthRest() + " " + getEighthNote() + " " + getEighthRest() + " " + getEighthNote() + " Rest + Eighth + Rest + Eighth",
                    static_cast<int> (Subdivision::RestEighthPattern) },

                // 12. Eight + Eight + Quarter
                { getEighthNote() + " " + getEighthNote() + " " + getQuarterNote() + " Two Eighth + Quarter",
                    static_cast<int> (Subdivision::EighthEighthQuarter) },

                // 13. Quarter + Eight + Eight
                { getQuarterNote() + " " + getEighthNote() + " " + getEighthNote() + " Quarter + Two Eighth",
                    static_cast<int> (Subdivision::QuarterEighthEighth) },

                // 14. Eight + Quarter + Eight
                { getEighthNote() + " " + getQuarterNote() + " " + getEighthNote() + " Eighth + Quarter + Eighth",
                    static_cast<int> (Subdivision::EighthQuarterEighth) }
            };
            break;

        case 4: // x/4 patterns
            patterns = {
                // 1. Quarter note
                { getQuarterNote() + " Quarter Note",
                    static_cast<int> (Subdivision::NoSubdivision) },

                // 2. Two eighth notes
                { getEighthNote() + " " + getEighthNote() + " Two Eighth Notes",
                    static_cast<int> (Subdivision::Half) },

                // 3. Eighth + Rest
                { getEighthNote() + " " + getEighthRest() + " Eighth + Rest",
                    static_cast<int> (Subdivision::HalfAndRest) },

                // 4. Rest + Eighth
                { getEighthRest() + " " + getEighthNote() + " Rest + Eighth",
                    static_cast<int> (Subdivision::RestHalf) },

                // 5. Triplet
                { getEighthNote() + " " + getEighthNote() + " " + getEighthNote() + " Triplet",
                    static_cast<int> (Subdivision::Triplet) },

                // 6. Rest + Eighth + Eighth (triplet)
                { getEighthRest() + " " + getEighthNote() + " " + getEighthNote() + " Rest + Two Eighth (Triplet)",
                    static_cast<int> (Subdivision::RestHalfHalfTriplet) },

                // 7. Eighth + Rest + Eighth (triplet)
                { getEighthNote() + " " + getEighthRest() + " " + getEighthNote() + " Eighth + Rest + Eighth (Triplet)",
                    static_cast<int> (Subdivision::HalfRestHalfTriplet) },

                // 8. Eighth + Eighth + Rest (triplet)
                { getEighthNote() + " " + getEighthNote() + " " + getEighthRest() + " Two Eighth + Rest (Triplet)",
                    static_cast<int> (Subdivision::HalfHalfRestTriplet) },

                // 9. Rest + Eighth + Rest (triplet)
                { getEighthRest() + " " + getEighthNote() + " " + getEighthRest() + " Rest + Eighth + Rest (Triplet)",
                    static_cast<int> (Subdivision::RestHalfRestTriplet) },

                // 10. Four Sixteenth Notes
                { getSixteenthNotes() + " Four Sixteenth Notes",
                    static_cast<int> (Subdivision::Quarter) },

                // 11. Rest + Sixteenth + Rest + Sixteenth
                { getSixteenthRest() + " " + getSixteenthNotes() + " " + getSixteenthRest() + " " + getSixteenthNotes() + " Rest + 16th + Rest + 16th",
                    static_cast<int> (Subdivision::RestEighthPattern) },

                // 12. Sixteenth + Sixteenth + Eighth
                { getSixteenthNotes() + " " + getSixteenthNotes() + " " + getEighthNote() + " Two 16th + Eighth",
                    static_cast<int> (Subdivision::EighthEighthQuarter) },

                // 13. Eighth + Sixteenth + Sixteenth
                { getEighthNote() + " " + getSixteenthNotes() + " " + getSixteenthNotes() + " Eighth + Two 16th",
                    static_cast<int> (Subdivision::QuarterEighthEighth) },

                // 14. Sixteenth + Eighth + Sixteenth
                { getSixteenthNotes() + " " + getEighthNote() + " " + getSixteenthNotes() + " 16th + Eighth + 16th",
                    static_cast<int> (Subdivision::EighthQuarterEighth) }
            };
            break;

        case 8: // x/8 patterns
            patterns = {
                // 1. Eighth note
                { getEighthNote() + " Eighth Note",
                    static_cast<int> (Subdivision::NoSubdivision) },

                // 2. Two Sixteenth notes
                { getSixteenthNotes() + " Two Sixteenth Notes",
                    static_cast<int> (Subdivision::Half) },

                // 3. Sixteenth + Rest
                { getSixteenthNotes() + " " + getSixteenthRest() + " Sixteenth + Rest",
                    static_cast<int> (Subdivision::HalfAndRest) },

                // 4. Rest + Sixteenth
                { getSixteenthRest() + " " + getSixteenthNotes() + " Rest + Sixteenth",
                    static_cast<int> (Subdivision::RestHalf) },

                // 5. Triplet
                { getSixteenthNotes() + " " + getSixteenthNotes() + " " + getSixteenthNotes() + " Triplet",
                    static_cast<int> (Subdivision::Triplet) },

                // 6. Rest + Sixteenth + Sixteenth (triplet)
                { getSixteenthRest() + " " + getSixteenthNotes() + " " + getSixteenthNotes() + " Rest + Two 16th (Triplet)",
                    static_cast<int> (Subdivision::RestHalfHalfTriplet) },

                // 7. Sixteenth + Rest + Sixteenth (triplet)
                { getSixteenthNotes() + " " + getSixteenthRest() + " " + getSixteenthNotes() + " 16th + Rest + 16th (Triplet)",
                    static_cast<int> (Subdivision::HalfRestHalfTriplet) },

                // 8. Sixteenth + Sixteenth + Rest (triplet)
                { getSixteenthNotes() + " " + getSixteenthNotes() + " " + getSixteenthRest() + " Two 16th + Rest (Triplet)",
                    static_cast<int> (Subdivision::HalfHalfRestTriplet) },

                // 9. Rest + Sixteenth + Rest (triplet)
                { getSixteenthRest() + " " + getSixteenthNotes() + " " + getSixteenthRest() + " Rest + 16th + Rest (Triplet)",
                    static_cast<int> (Subdivision::RestHalfRestTriplet) },

                // 10. Four 32nd Notes
                { getTwoEighthNotes() + " Four 32nd Notes",
                    static_cast<int> (Subdivision::Quarter) },

                // 11. Rest + 32nd + Rest + 32nd
                { getSixteenthRest() + " " + getTwoEighthNotes() + " " + getSixteenthRest() + " " + getTwoEighthNotes() + " Rest + 32nd + Rest + 32nd",
                    static_cast<int> (Subdivision::RestEighthPattern) },

                // 12. Two 32nd + Sixteenth
                { getTwoEighthNotes() + " " + getTwoEighthNotes() + " " + getSixteenthNotes() + " Two 32nd + 16th",
                    static_cast<int> (Subdivision::EighthEighthQuarter) },

                // 13. Sixteenth + Two 32nd
                { getSixteenthNotes() + " " + getTwoEighthNotes() + " " + getTwoEighthNotes() + " 16th + Two 32nd",
                    static_cast<int> (Subdivision::QuarterEighthEighth) },

                // 14. 32nd + Sixteenth + 32nd
                { getTwoEighthNotes() + " " + getSixteenthNotes() + " " + getTwoEighthNotes() + " 32nd + 16th + 32nd",
                    static_cast<int> (Subdivision::EighthQuarterEighth) }
            };
            break;

        default:
            break;
    }

    return patterns;
}