#pragma once

#include <chrono>

class Time
{
    friend class Core;

    public:
    static float DeltaTime();
    static float DoubleDeltaTime();
    static float TimeSinceInit();

    private:
    static float deltaTime;
    static std::chrono::time_point<std::chrono::high_resolution_clock> loopStartTime;
    static std::chrono::time_point<std::chrono::high_resolution_clock> loopEndTime;
    static std::chrono::time_point<std::chrono::high_resolution_clock> initTime;

    static void Initialize();
    static void CalculateLoopTime();
};
