#include "RenderQueue.h"

std::queue<ObjectRenderCommand> RenderQueue::queue;
GlobalRenderCommand RenderQueue::onceCommand;
bool RenderQueue::hasOnceCommand = false;

void RenderQueue::Enqueue(const ObjectRenderCommand& command) {
    queue.push(command);
}

ObjectRenderCommand* RenderQueue::Dequeue() {
    if (!queue.empty()) {
        ObjectRenderCommand* command = &queue.front();
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
    while (!queue.empty()) {
        queue.pop();
    }
}

void RenderQueue::EnqueueOnce(const GlobalRenderCommand& command) {
    onceCommand = command;
    hasOnceCommand = true;
}

const GlobalRenderCommand* RenderQueue::GetOnceCommand() {
    if (hasOnceCommand) {
        hasOnceCommand = false; // Reset flag after fetching
        return &onceCommand;
    }
    return nullptr;
}
