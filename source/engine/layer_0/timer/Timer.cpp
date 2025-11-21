#include "Timer.h"

float Timer::deltaTime = 0.0f;
float Timer::fixedTimeStep = 0.02f;
float Timer::accumulatedTime = 0.0f;
float Timer::maximumAllowedTimeStep = 0.1f;

timePoint Timer::loopStartTime;
timePoint Timer::loopEndTime;
timePoint Timer::initTime;
timePoint Timer::pauseTime;

void Timer::Initialize()
{
    initTime = loopStartTime = resClock::now();
}

void Timer::Pause()
{
    pauseTime = resClock::now();
}

void Timer::Resume()
{
    timePoint resumeTime = resClock::now();
    timeDuration pausedDuration = resumeTime - pauseTime;

    // ToDo - Ugly fix. Make it +=
    loopStartTime = resumeTime;
    loopEndTime = resumeTime;
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
    return std::chrono::duration<float>(resClock::now() - initTime).count();
}

float Timer::TimeSinceEpoch()
{
    return std::chrono::duration<float>(resClock::now().time_since_epoch()).count();
}

void Timer::CalculateLoopTime()
{
    loopEndTime = resClock::now();
    timeDuration delta = loopEndTime - loopStartTime;
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
