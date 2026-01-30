#pragma once

#include "Camera.h"
#include "Timer.h"
#include "Input.h"
#include "Entity.h"
#include "CameraController.h"
#include "Debug.h"

void CameraController::OnCreate()
{
    m_camera = GetEntity().GetComponent<Camera>();

    if (m_camera == nullptr)
    {
        Debug::LogError("No Camera on entity " + std::to_string(GetEntity().GetId()));
    }
}

void CameraController::OnDestroy()
{
    m_camera = nullptr;
}

void CameraController::Tick()
{
    if (m_camera)
    {
        HandleCamMovement();
    }
}

void CameraController::HandleCamMovement()
{
    float deltaTime = Timer::DeltaTime();

    // Move forward
    if (Input::GetKey("w"))
    {
        m_camera->GetTransform().position += m_camera->GetTransform().getForward() * m_cameraMoveSpeed * deltaTime;
    }

    // Move back
    if (Input::GetKey("s"))
    {
        m_camera->GetTransform().position -= m_camera->GetTransform().getForward() * m_cameraMoveSpeed * deltaTime;
    }

    // Look left
    if (Input::GetKey("d"))
    {
        m_camera->GetTransform().eulerAngles += Vector3(0, m_cameraLookSpeed * deltaTime, 0);
    }

    // Look right
    if (Input::GetKey("a"))
    {
        m_camera->GetTransform().eulerAngles -= Vector3(0, m_cameraLookSpeed * deltaTime, 0);
    }

    // Look up
    if (Input::GetKey("e"))
    {
        m_camera->GetTransform().eulerAngles += Vector3(m_cameraLookSpeed * deltaTime, 0, 0);
    }

    // Look down
    if (Input::GetKey("q"))
    {
        m_camera->GetTransform().eulerAngles -= Vector3(m_cameraLookSpeed * deltaTime, 0, 0);
    }
}
