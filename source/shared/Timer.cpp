#include "Timer.h"

float Timer::deltaTime = 0.0;
std::chrono::time_point<std::chrono::high_resolution_clock> Timer::loopStartTime;
std::chrono::time_point<std::chrono::high_resolution_clock> Timer::loopEndTime;
std::chrono::time_point<std::chrono::high_resolution_clock> Timer::initTime;

void Timer::Initialize()
{
    initTime = loopStartTime = std::chrono::high_resolution_clock::now();
}

void Timer::CalculateLoopTime()
{
    loopEndTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> delta = loopEndTime - loopStartTime;
    deltaTime = delta.count();
    loopStartTime = loopEndTime;  // Prepare for the next loop
}

float Timer::DeltaTime()
{
    return deltaTime;
}

float Timer::DoubleDeltaTime()
{
    return 2.0f * deltaTime;
}

float Timer::TimeSinceInit()
{
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = now - initTime;
    return elapsed.count();
}
