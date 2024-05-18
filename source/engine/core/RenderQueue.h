#pragma once

#include <vector>
#include "RenderCommand.h"

class RenderQueue
{
    public:
    static const GlobalRenderCommand* GetGlobalRenderCommand();
    static const std::vector<ObjectRenderCommand>& GetObjectRenderCommands();
    static const std::vector<LightRenderCommand>& GetLightRenderCommands();

    static void Clear();
    static void AddGlobalRenderCommand(const GlobalRenderCommand& command);
    static void AddObjectRenderCommand(const ObjectRenderCommand& command);
    static void AddLightRenderCommand(const LightRenderCommand& command);
    
    private:
    static GlobalRenderCommand globalRenderCommand;
    static std::vector<ObjectRenderCommand> objectRenderCommands;
    static std::vector<LightRenderCommand> lightRenderCommands;
};