#include "TimeTypes.h"

class Stopwatch
{
    public:
    void start();
    void stop();
    void reset();
    void restart();
    bool isRunning() const;
    long long getElapsedSeconds() const;
    long long getElapsedMilliseconds() const;
    long long getElapsedMicroseconds() const;

    private:
    Duration getElapsedTime() const;

    private:
    bool m_isRunning = false;
    TimePoint m_startTime;
    Duration m_elapsedTime = Duration::zero();
};