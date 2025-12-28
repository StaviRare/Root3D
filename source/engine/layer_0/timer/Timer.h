#pragma once
#include "TimeTypes.h"
#include "TimeDesc.h"

class Timer
{
    friend class Engine;

    public:
    static float DeltaTime();
    static float FixedDeltaTime();
    static float TimeSinceInit();
    static float TimeSinceEpoch();
    static float AccumulatedTime();

    private:
    static void Initialize(TimeDesc desc);
    static void Pause();
    static void Resume();
    static void CalculateLoopTime();
    static void UpdateFixedTime();

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