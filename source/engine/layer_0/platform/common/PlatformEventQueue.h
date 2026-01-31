#pragma once

#include <queue>
#include <mutex>

#include "Types.h"

// No Pause/Resume because we want to handle it right away
// Either rename to windowEvents or handle it better in poll events

enum class EventType
{
    Resize,
    SurfaceCreated,
    SurfaceDestroyed,
    Close,
    FocusGained,
    FocusLost,
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