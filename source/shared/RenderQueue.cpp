#include "RenderQueue.h"

GlobalRenderCommand RenderQueue::globalRenderCommand;
std::vector<ObjectRenderCommand> RenderQueue::objectRenderCommands;
std::vector<LightRenderCommand> RenderQueue::lightRenderCommands;

void RenderQueue::Clear()
{
    objectRenderCommands.clear();
    lightRenderCommands.clear();
}

void RenderQueue::AddGlobalRenderCommand(const GlobalRenderCommand& command)
{
    globalRenderCommand = command;
}

void RenderQueue::AddObjectRenderCommand(const ObjectRenderCommand& command)
{
    objectRenderCommands.push_back(command);
}

void RenderQueue::AddLightRenderCommand(const LightRenderCommand& command)
{
    lightRenderCommands.push_back(command);
}

const GlobalRenderCommand* RenderQueue::GetGlobalRenderCommand()
{
    return &globalRenderCommand;
}

const std::vector<ObjectRenderCommand>& RenderQueue::GetObjectRenderCommands()
{
    return objectRenderCommands;
}

const std::vector<LightRenderCommand>& RenderQueue::GetLightRenderCommands()
{
    return lightRenderCommands;
}