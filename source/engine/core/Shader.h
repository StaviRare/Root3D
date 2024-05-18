#pragma once

#include <string>

class Shader 
{
public:
    unsigned int ID;
    std::string vertexCode;
    std::string fragmentCode;

    Shader(const std::string& vertexSource, const std::string& fragmentSource)
        : vertexCode(vertexSource), fragmentCode(fragmentSource), ID(0) {}

    Shader() : ID(0) {}
};

