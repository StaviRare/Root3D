#include "Log.h"
#include "Camera.h"
#include "Screen.h"

Camera* Camera::Instance = nullptr;

Camera::Camera()
{
    if (Instance == nullptr)
    {
        Instance = this;
    }
    else
    {
        ENGINE_ERROR("Multiple instances of Camera are not supported!");
    }
}

Camera::~Camera()
{
    if (Instance == this)
    {
        Instance = nullptr;
    }
}

bool Camera::Exists()
{
    return Instance != nullptr;
}

Camera& Camera::GetInstance()
{
    if (Instance == nullptr)
    {
        ENGINE_ERROR("Camera instance does not exists!");
    }

    return *Instance;
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
        returnValue = static_cast<float>( Screen::GetWidth() ) / Screen::GetHeight();
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