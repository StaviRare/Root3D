#include "Camera.h"
#include "Debug.h"
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
        Debug::LogError("Multiple instances of Camera are not supported!");
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
        Debug::LogError("Camera instance does not exists!");
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