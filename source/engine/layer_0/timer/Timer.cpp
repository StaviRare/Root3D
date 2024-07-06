#include "Timer.h"

float Timer::deltaTime = 0.0f;
float Timer::fixedTimeStep = 0.02f;
float Timer::accumulatedTime = 0.0f;
float Timer::maximumAllowedTimeStep = 0.1f;

std::chrono::time_point<std::chrono::high_resolution_clock> Timer::loopStartTime;
std::chrono::time_point<std::chrono::high_resolution_clock> Timer::loopEndTime;
std::chrono::time_point<std::chrono::high_resolution_clock> Timer::initTime;

void Timer::Initialize()
{
    initTime = loopStartTime = std::chrono::high_resolution_clock::now();
}

float Timer::DeltaTime()
{
    return deltaTime;
}

float Timer::FixedDeltaTime()
{
    return fixedTimeStep;
}

float Timer::TimeSinceInit()
{
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = now - initTime;
    return elapsed.count();
}

void Timer::CalculateLoopTime()
{
    loopEndTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> delta = loopEndTime - loopStartTime;
    deltaTime = delta.count();
    loopStartTime = loopEndTime;

    accumulatedTime += deltaTime;

    // Cap accumulated time to prevent long physics steps
    if (accumulatedTime > maximumAllowedTimeStep)
    {
        accumulatedTime = maximumAllowedTimeStep;
    }
}

void Timer::UpdateFixedTime()
{
    accumulatedTime -= fixedTimeStep;
}
