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
    void BindTexture(Texture& texture) override;

private:
    std::list<unsigned int> shaderProgramIDs;
    unsigned int CompileShader(const string& source, unsigned int type);
    unsigned int CreateShaderProgram(const string& vertexSource, const string& fragmentSource);
};
