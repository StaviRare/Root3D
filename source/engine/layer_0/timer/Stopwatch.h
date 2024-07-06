#pragma once

#include <chrono>

class Stopwatch {
public:
    Stopwatch();

    void Start();
    void Stop();
    bool IsRunning() const;
    void Reset();
    long long ElapsedMilliseconds() const;

private:
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point end_time;
    bool running;
};
