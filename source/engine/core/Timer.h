#pragma once
#include <chrono>

class Timer
{
    friend class Core;

    public:
    static void Initialize();
    static void CalculateLoopTime();
    static float DeltaTime();
    static float FixedDeltaTime();
    static float TimeSinceInit();
    static void UpdateFixedTime();

    private:
    static float deltaTime;
    static float fixedTimeStep;
    static float accumulatedTime;
    static float maximumAllowedTimeStep;

    static std::chrono::time_point<std::chrono::high_resolution_clock> loopStartTime;
    static std::chrono::time_point<std::chrono::high_resolution_clock> loopEndTime;
    static std::chrono::time_point<std::chrono::high_resolution_clock> initTime;
    static std::chrono::time_point<std::chrono::high_resolution_clock> fixedUpdateStartTime;
};
