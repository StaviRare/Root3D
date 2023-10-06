#include "Timer.h"

std::chrono::time_point<std::chrono::high_resolution_clock> Timer::loopStartTime;
std::chrono::time_point<std::chrono::high_resolution_clock> Timer::loopEndTime;
std::chrono::time_point<std::chrono::high_resolution_clock> Timer::initTime;
double Timer::deltaTime = 0.0;

void Timer::initialize()
{
    initTime = loopStartTime = std::chrono::high_resolution_clock::now();
}

void Timer::calculateLoopTime()
{
    loopEndTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> delta = loopEndTime - loopStartTime;
    deltaTime = delta.count();
    loopStartTime = loopEndTime;  // Prepare for the next loop
}

double Timer::getDeltaTime()
{
    return deltaTime;
}

double Timer::getDoubleDeltaTime()
{
    return 2.0 * deltaTime;
}

double Timer::getTimeSinceInit()
{
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = now - initTime;
    return elapsed.count();
}
