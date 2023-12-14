#include "Time.h"

std::chrono::time_point<std::chrono::high_resolution_clock> Time::loopStartTime;
std::chrono::time_point<std::chrono::high_resolution_clock> Time::loopEndTime;
std::chrono::time_point<std::chrono::high_resolution_clock> Time::initTime;
double Time::deltaTime = 0.0;

void Time::Initialize()
{
    initTime = loopStartTime = std::chrono::high_resolution_clock::now();
}

void Time::CalculateLoopTime()
{
    loopEndTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> delta = loopEndTime - loopStartTime;
    deltaTime = delta.count();
    loopStartTime = loopEndTime;  // Prepare for the next loop
}

double Time::DeltaTime()
{
    return deltaTime;
}

double Time::DoubleDeltaTime()
{
    return 2.0 * deltaTime;
}

double Time::TimeSinceInit()
{
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = now - initTime;
    return elapsed.count();
}
