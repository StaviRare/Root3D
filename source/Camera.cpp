#include "Camera.h"
#include "Debug.h"
#include <iostream>

Camera* Camera::instance = nullptr;

Camera::Camera()
{
    if (instance == nullptr)
    {
        instance = this;
    }
    else
    {
        Debug::error("Error: Multiple instances of Camera are not supported.");
        exit(1);
    }
}

bool Camera::exists()
{
    return instance != nullptr;
}

Camera& Camera::getInstance()
{
    if (instance == nullptr)
    {
        Debug::error("Error: No instance of Camera exists.");
        exit(1);
    }

    return *instance;
}
