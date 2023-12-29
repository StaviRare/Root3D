#pragma once
#include "GraphicsAPI.h"
#include <string>
#include <iostream>
#include <GL/glew.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <list>

class OpenGLAPI : public GraphicsAPI
{
public:
    OpenGLAPI();
    ~OpenGLAPI();

    void Initialize() override;
    void ClearScreen() override;
    void ExecuteRenderCommands() override;
    void BindTexture(Texture& texture) override;

private:
    std::list<GLuint> shaderProgramIDs;
    GLuint CompileShader(const std::string& source, GLenum type);
    GLuint CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);
};
