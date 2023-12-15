#include "RenderQueue.h"

std::queue<RenderCommand> RenderQueue::queue;

void RenderQueue::Enqueue(const RenderCommand& command) {
    queue.push(command);
}

RenderCommand* RenderQueue::Dequeue() {
    if (!queue.empty()) {
        RenderCommand* command = &queue.front();
        queue.pop();
        return command;
    }
    return nullptr;
}

bool RenderQueue::IsEmpty() {
    return queue.empty();
}

size_t RenderQueue::Size() {
    return queue.size();
}

void RenderQueue::Clear() {
    std::queue<RenderCommand> empty;
    std::swap(queue, empty);
}
