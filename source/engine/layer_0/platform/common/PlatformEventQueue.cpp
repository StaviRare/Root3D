#include "PlatformEventQueue.h"

std::mutex PlatformEventQueue::s_queueMutex;
std::queue<PlatformEvent> PlatformEventQueue::s_eventQueue;

void PlatformEventQueue::Push(const PlatformEvent ev)
{
    std::lock_guard<std::mutex> lock(s_queueMutex);
    s_eventQueue.push(ev);
}

bool PlatformEventQueue::Poll(PlatformEvent& outEvent)
{
    std::lock_guard<std::mutex> lock(s_queueMutex);

    if (!s_eventQueue.empty()) 
    {
        outEvent = s_eventQueue.front();
        s_eventQueue.pop();
        return true;
    }

    return false;
}