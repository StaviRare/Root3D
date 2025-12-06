#pragma once
#include "TimeTypes.h"

class Timer
{
    friend class Engine;

    private:
    static void Initialize();
    static void Pause();
    static void Resume();
    static void CalculateLoopTime();
    static void UpdateFixedTime();

    public:
    static float DeltaTime();
    static float FixedDeltaTime();
    static float TimeSinceInit();
    static float TimeSinceEpoch();
    static float AccumulatedTime();
    static void SetFixedStep(float seconds);
    static void SetMaximumStep(float seconds);

    private:
    static bool s_paused;
    static Duration s_delta;
    static Duration s_accumulated;
    static Duration s_fixedStep;
    static Duration s_maxStep;
    static TimePoint s_startTime;
    static TimePoint s_lastTime;
    static TimePoint s_pauseTime;
};