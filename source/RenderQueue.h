#pragma once

#include <queue>
#include <memory>
#include "RenderCommand.h"

class RenderQueue {
public:
    static void Enqueue(const RenderCommand& command);
    static RenderCommand* Dequeue();
    static bool IsEmpty();
    static size_t Size();
    static void Clear();

private:
    static std::queue<RenderCommand> queue;
};