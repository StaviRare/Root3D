#pragma once

#include <chrono>

class Time
{
private:
    static std::chrono::time_point<std::chrono::high_resolution_clock> loopStartTime;
    static std::chrono::time_point<std::chrono::high_resolution_clock> loopEndTime;
    static std::chrono::time_point<std::chrono::high_resolution_clock> initTime;
    static float deltaTime;


public:
    static void Initialize();
    static void CalculateLoopTime();
    static float DeltaTime();
    static float DoubleDeltaTime();
    static float TimeSinceInit();
};
