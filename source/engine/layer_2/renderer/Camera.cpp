#include "Log.h"
#include "Camera.h"
#include "Screen.h"

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