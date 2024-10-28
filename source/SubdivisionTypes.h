#pragma once

/**
 * @brief Subdivision types for all time signature denominators
 * Subdivisions are relative to the current beat length
 */
enum class Subdivision {
    NoSubdivision, /**< Single note per beat */
    Half, /**< Two notes per beat */
    HalfAndRest, /**< One note + one rest of equal length */
    RestHalf, /**< One rest + one note */
    Triplet, /**< Three notes per beat */
    RestHalfHalfTriplet, /**< Rest + two notes in triplet */
    HalfRestHalfTriplet, /**< Note + rest + note in triplet */
    HalfHalfRestTriplet, /**< Two notes + rest in triplet */
    RestHalfRestTriplet, /**< Rest + note + rest in triplet */
    Quarter, /**< Four notes per beat */
    RestEighthPattern, /**< Rest + note + rest + note pattern */
    EighthEighthQuarter, /**< Two shorter notes + longer note */
    QuarterEighthEighth, /**< Longer note + two shorter notes */
    EighthQuarterEighth, /**< Short + long + short pattern */
    Count /**< Number of subdivisions */
};

/** @brief Total number of available subdivisions */
constexpr int SubdivisionCount = static_cast<int>(Subdivision::Count);
