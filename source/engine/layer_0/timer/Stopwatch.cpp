#include "Stopwatch.h"

void Stopwatch::start()
{
    if (!m_isRunning)
    {
        m_startTime = HighResClock::now();
        m_isRunning = true;
    }
}

void Stopwatch::stop()
{
    if (m_isRunning)
    {
        auto endTime = HighResClock::now();
        m_elapsedTime += endTime - m_startTime;
        m_isRunning = false;
    }
}

void Stopwatch::reset()
{
    stop();
    m_elapsedTime = Duration::zero();
}

void Stopwatch::restart()
{
    reset();
    start();
}

bool Stopwatch::isRunning() const
{
    return m_isRunning;
}

long long Stopwatch::getElapsedSeconds() const
{
    return std::chrono::duration_cast<Seconds>( getElapsedTime() ).count();
}

long long Stopwatch::getElapsedMilliseconds() const
{
    return std::chrono::duration_cast<Milliseconds>( getElapsedTime() ).count();
}

long long Stopwatch::getElapsedMicroseconds() const
{
    return std::chrono::duration_cast<Microseconds>( getElapsedTime() ).count();
}

Duration Stopwatch::getElapsedTime() const
{
    if (m_isRunning)
    {
        auto now = HighResClock::now();
        return m_elapsedTime + ( now - m_startTime );
    }
    return m_elapsedTime;
}