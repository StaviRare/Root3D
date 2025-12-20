#include "Log.h"
#include "Camera.h"
#include "Window.h"

Camera* Camera::instance = nullptr;

void Camera::OnCreate()
{
    if (instance == nullptr)
    {
        instance = this;
    }
    else
    {
        ENGINE_ERROR("Multiple instances of Camera are not supported!");
    }
}

void Camera::OnDestroy()
{
    if (instance == this)
    {
        instance = nullptr;
    }
}

bool Camera::Exists()
{
    return instance != nullptr;
}

Camera* Camera::GetInstance()
{
    if (instance == nullptr)
    {
        ENGINE_ERROR("Camera instance does not exists!");
    }

    return instance;
}

float Camera::GetAspect()
{
    float returnValue;

    if (isUsingCustomAspect)
    {
        returnValue = customAspect;
    }
    else
    {
        Window* window = Window::getInstance();
        
        if (window)
        {
            returnValue = static_cast<float>( window->GetWidth() ) / window->GetHeight();
        }
        else
        {
            returnValue = 1;
            Log::Error("could nto find window.");
        }
    }

    return returnValue;
}

void Camera::ResetAspect()
{
    isUsingCustomAspect = false;
}

void Camera::SetAspect(float aspect)
{
    customAspect = aspect;
    isUsingCustomAspect = true;
}