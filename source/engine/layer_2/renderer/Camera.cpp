#include "Log.h"
#include "Camera.h"
#include "Window.h"

std::vector<Camera*> Camera::s_cameras;

bool Camera::Exists()
{
    return s_cameras.size() > 0;
}

Camera* Camera::GetMainCamera()
{
    Camera* returnValue = nullptr;

    if (s_cameras.empty() == false)
    {
        returnValue = s_cameras[0];
        int highestDepth = s_cameras[0]->depth;

        for (size_t i = 1; i < s_cameras.size(); i++)
        {
            if (s_cameras[i]->depth > highestDepth)
            {
                highestDepth = s_cameras[i]->depth;
                returnValue = s_cameras[i];
            }
        }
    }
    else
    {
        ENGINE_ERROR("No Camera exists!");
    }

    return returnValue;
}

void Camera::OnCreate()
{
    s_cameras.push_back(this);
}

void Camera::OnDestroy()
{
    auto it = std::find(s_cameras.begin(), s_cameras.end(), this);

    if (it != s_cameras.end())
    {
        s_cameras.erase(it);
    }
}

float Camera::GetAspect()
{
    float returnValue;

    if (m_isUsingCustomAspect)
    {
        returnValue = m_customAspect;
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
    m_isUsingCustomAspect = false;
}

void Camera::SetAspect(float aspect)
{
    m_customAspect = aspect;
    m_isUsingCustomAspect = true;
}