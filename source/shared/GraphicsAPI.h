#pragma once

class GraphicsAPI
{
    public:
    virtual void Initialize() = 0;
    virtual void ClearScreen() = 0;
    virtual void ExecuteRenderCommands() = 0;
    virtual void UnInitialize() = 0;

    protected:
    bool initialized = false;
};

