#pragma once
#include <chrono>

using resClock = std::chrono::high_resolution_clock;
using timePoint = std::chrono::time_point<resClock>;
using timeDuration = std::chrono::duration<float>;

class Timer
{
    friend class EngineRuntime;

    public:
    static float DeltaTime();
    static float FixedDeltaTime();
    static float TimeSinceInit();

    private:
    static float deltaTime;
    static float fixedTimeStep;
    static float accumulatedTime;
    static float maximumAllowedTimeStep;
    static timePoint pauseTime;
    static timePoint loopStartTime;
    static timePoint loopEndTime;
    static timePoint initTime;
    static timePoint fixedUpdateStartTime;

    private:
    static void Initialize();
    static void Pause();
    static void Resume();
    static void CalculateLoopTime();
    static void UpdateFixedTime();
    static float TimeSinceEpoch();
};
