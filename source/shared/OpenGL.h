#pragma once
#include <list>

#include "Types.h"
#include "GraphicsAPI.h"

class OpenGL : public GraphicsAPI
{
public:
    OpenGL();
    ~OpenGL();

    void Initialize() override;
    void ClearScreen() override;
    void ExecuteRenderCommands() override;
    void UnInitialize() override;

private:
    std::list<unsigned int> shaderProgramIDs;
    void BindTexture(Texture& texture);
    static void OnWindowResize(int width, int height);
    unsigned int CompileShader(const string& source, unsigned int type);
    unsigned int CreateShaderProgram(const string& vertexSource, const string& fragmentSource);
};
