#pragma once

#include "Camera.h"
#include "Timer.h"
#include "Input.h"
#include "Entity.h"
#include "CameraController.h"
#include "Debug.h"

static Camera* camera;
static float cameraMoveSpeed = 3.00f;
static float cameraLookSpeed = 2.00f;

void CameraController::OnCreate()
{
    camera = GetEntity().GetComponent<Camera>();

    if (camera == nullptr)
    {
        Debug::LogError("No Camera on entity " + std::to_string(GetEntity().GetId()));
    }
    else
    {
        camera->fov = 60.0f;
        camera->backgroundColor = Color(0.1f, 0, 0.1f, 0);
        camera->GetTransform().position = Vector3(0, 0, 4);
    }
}

void CameraController::OnDestroy()
{

}

void CameraController::Tick()
{
    if (camera)
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
        camera->GetTransform().position += camera->GetTransform().getForward() * cameraMoveSpeed * deltaTime;
    }

    // Move back
    if (Input::GetKey("s"))
    {
        camera->GetTransform().position -= camera->GetTransform().getForward() * cameraMoveSpeed * deltaTime;
    }

    // Look left
    if (Input::GetKey("d"))
    {
        camera->GetTransform().eulerAngles += Vector3(0, cameraLookSpeed * deltaTime, 0);
    }

    // Look right
    if (Input::GetKey("a"))
    {
        camera->GetTransform().eulerAngles -= Vector3(0, cameraLookSpeed * deltaTime, 0);
    }

    // Look up
    if (Input::GetKey("e"))
    {
        camera->GetTransform().eulerAngles += Vector3(cameraLookSpeed * deltaTime, 0, 0);
    }

    // Look down
    if (Input::GetKey("q"))
    {
        camera->GetTransform().eulerAngles -= Vector3(cameraLookSpeed * deltaTime, 0, 0);
    }
}
