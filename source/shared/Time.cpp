#include "Time.h"

std::chrono::time_point<std::chrono::high_resolution_clock> Time::loopStartTime;
std::chrono::time_point<std::chrono::high_resolution_clock> Time::loopEndTime;
std::chrono::time_point<std::chrono::high_resolution_clock> Time::initTime;
float Time::deltaTime = 0.0;

void Time::Initialize()
{
    initTime = loopStartTime = std::chrono::high_resolution_clock::now();
}

void Time::CalculateLoopTime()
{
    loopEndTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> delta = loopEndTime - loopStartTime;
    deltaTime = delta.count();
    loopStartTime = loopEndTime;  // Prepare for the next loop
}

float Time::DeltaTime()
{
    return deltaTime;
}

float Time::DoubleDeltaTime()
{
    return 2.0f * deltaTime;
}

float Time::TimeSinceInit()
{
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = now - initTime;
    return elapsed.count();
}
