#pragma once

#include <chrono>

class Timer
{
private:
    static std::chrono::time_point<std::chrono::high_resolution_clock> loopStartTime;
    static std::chrono::time_point<std::chrono::high_resolution_clock> loopEndTime;
    static std::chrono::time_point<std::chrono::high_resolution_clock> initTime;
    static double deltaTime;


public:
    static void initialize();
    static void calculateLoopTime();
    static double getDeltaTime();
    static double getDoubleDeltaTime();
    static double getTimeSinceInit();
};
