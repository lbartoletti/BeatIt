#pragma once

/**
 * @brief Subdivision types for all time signature denominators
 * Subdivisions are relative to the current beat length
 */
enum class Subdivision
{
    NoSubdivision,    /**< Single note per beat */
    Half,             /**< Two notes per beat */
    HalfAndRest,      /**< One note + one rest of equal length */
    Triplet,          /**< Three notes per beat */
    Quarter,          /**< Four notes per beat */
    HalfQuarter,      /**< One half + two quarters (e.g., eighth + two sixteenths) */
    QuarterHalf,      /**< Two quarters + one half (e.g., two sixteenths + eighth) */
    Count             /**< Number of subdivisions */
};

/** @brief Total number of available subdivisions */
constexpr int SubdivisionCount = static_cast<int>(Subdivision::Count);
