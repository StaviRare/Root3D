#pragma once

#include <cstdint>
#include <string>

// Define primitive data types for the Platform Independence Layer (PIL).
// These types will replace the larger data types previously used in the PIL and the renderCommand.

// Integers
using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

// Floating points
using float32 = float;
using float64 = double;

// Others
using hash = size_t;
using byte = uint8;
using uniqueID  = uint64;
using string = std::string;
