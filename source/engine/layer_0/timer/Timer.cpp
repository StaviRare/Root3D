#include "Timer.h"

bool Timer::s_paused = false;
TimePoint Timer::s_startTime;
TimePoint Timer::s_lastTime;
TimePoint Timer::s_pauseTime;
Duration Timer::s_delta = Duration::zero();
Duration Timer::s_accumulated = Duration::zero();
Duration Timer::s_maxStep = Duration::zero();
Duration Timer::s_fixedStep = Duration::zero();

void Timer::Initialize(TimeDesc desc)
{
    s_maxStep = std::chrono::duration_cast<Duration>( FloatDuration(desc.maxDeltaTime) );
    s_fixedStep = std::chrono::duration_cast<Duration>( FloatDuration(desc.fixedTimeStep) );
    s_startTime = s_lastTime = HighResClock::now();
    s_delta = Duration::zero();
    s_accumulated = Duration::zero();
    s_paused = false;
}

void Timer::Pause()
{
    if (s_paused == false)
    {
        s_pauseTime = HighResClock::now();
        s_paused = true;
    }
}

void Timer::Resume()
{
    if (s_paused)
    {
        Duration pausedDuration = HighResClock::now() - s_pauseTime;
        s_startTime += pausedDuration;
        s_lastTime += pausedDuration;
        s_paused = false;
    }
}

void Timer::CalculateLoopTime()
{
    if (s_paused == false)
    {
        TimePoint now = HighResClock::now();
        s_delta = std::chrono::duration_cast<Duration>( now - s_lastTime );
        s_lastTime = now;

        s_accumulated += s_delta;
        if (s_accumulated > s_maxStep)
        {
            s_accumulated = s_maxStep;
        }
    }
}

void Timer::UpdateFixedTime()
{
    s_accumulated -= s_fixedStep;
}

float Timer::DeltaTime()
{
    return FloatDuration(s_delta).count();
}

float Timer::FixedDeltaTime()
{
    return FloatDuration(s_fixedStep).count();
}

float Timer::TimeSinceInit()
{
    return FloatDuration(HighResClock::now() - s_startTime).count();
}

float Timer::TimeSinceEpoch()
{
    return FloatDuration(HighResClock::now().time_since_epoch()).count();
}

float Timer::AccumulatedTime()
{
    return FloatDuration(s_accumulated).count();
}