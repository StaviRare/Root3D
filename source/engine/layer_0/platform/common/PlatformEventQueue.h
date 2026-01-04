#pragma once

#include <queue>
#include <mutex>

#include "Types.h"

enum class EventType
{
    Resize,
    SurfaceCreated,
    SurfaceDestroyed,
    Close,
    FocusGained,
    FocusLost,
    Pause,
    Resume
};

struct PlatformEvent
{
    uint32 width;
    uint32 height;
    EventType type;
};

class PlatformEventQueue
{
    public:
    static void Push(const PlatformEvent ev);
    static bool Poll(PlatformEvent& outEvent);

    private:
    static std::mutex s_queueMutex;
    static std::queue<PlatformEvent> s_eventQueue;
};