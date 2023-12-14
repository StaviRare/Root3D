#pragma once

#include <chrono>

class Time
{
private:
    static std::chrono::time_point<std::chrono::high_resolution_clock> loopStartTime;
    static std::chrono::time_point<std::chrono::high_resolution_clock> loopEndTime;
    static std::chrono::time_point<std::chrono::high_resolution_clock> initTime;
    static double deltaTime;


public:
    static void Initialize();
    static void CalculateLoopTime();
    static double DeltaTime();
    static double DoubleDeltaTime();
    static double TimeSinceInit();
};
