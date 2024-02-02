// RenderQueue.h
#pragma once

#include <queue>
#include "RenderCommand.h"

class RenderQueue {
public:
    static void Enqueue(const ObjectRenderCommand& command);
    static ObjectRenderCommand* Dequeue();
    static bool IsEmpty();
    static size_t Size();
    static void Clear();

    static void EnqueueOnce(const GlobalRenderCommand& onceCommand);
    static const GlobalRenderCommand* GetOnceCommand();

private:
    static std::queue<ObjectRenderCommand> queue;
    static GlobalRenderCommand onceCommand;
    static bool hasOnceCommand;
};
