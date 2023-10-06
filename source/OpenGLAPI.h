#pragma once
#include "GraphicsAPI.h"

class OpenGLAPI : public GraphicsAPI
{
public:
    OpenGLAPI();
    ~OpenGLAPI();

    void Initialize() override;
    void ClearScreen() override;
    void ExecuteRenderCommands() override;
};
