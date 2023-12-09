#include "Stopwatch.h"

Stopwatch::Stopwatch()
    : running(false) {
}

void Stopwatch::Start() 
{
    if (!running) 
    {
        start_time = std::chrono::steady_clock::now();
        running = true;
    }
}

void Stopwatch::Stop() 
{
    if (running) 
    {
        end_time = std::chrono::steady_clock::now();
        running = false;
    }
}

bool Stopwatch::IsRunning() const 
{
    return running;
}

void Stopwatch::Reset() 
{
    running = false;
    start_time = std::chrono::steady_clock::now();
}

long long Stopwatch::ElapsedMilliseconds() const 
{
    if (running) 
    {
        auto current_time = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
    }
    else 
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    }
}
