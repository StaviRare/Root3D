#pragma once
#include <chrono>

using HighResClock = std::chrono::high_resolution_clock;

using Duration = std::chrono::nanoseconds;
using FloatDuration = std::chrono::duration<float>;
using TimePoint = std::chrono::time_point<HighResClock>;

using Seconds = std::chrono::seconds;
using Milliseconds = std::chrono::milliseconds;
using Microseconds = std::chrono::microseconds;