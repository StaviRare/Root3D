#include "ExampleScene.h"
#include "Camera.h"
#include "Timer.h"
#include "Input.h"

static Camera camera;
static float cameraMoveSpeed = 3.00f;
static float cameraLookSpeed = 2.00f;

void initCamera();
void handleCameraMovement();

void ExampleScene::init()
{
    initCamera();
}

void ExampleScene::tick()
{
    handleCameraMovement();
}

void initCamera()
{
    camera.fov = 60.0f;
    camera.position = Vector3(0, 0, 3);
}

void handleCameraMovement()
{
    double deltaTime = Timer::getDeltaTime();

    if (Input::keyDown("d"))
    {
        camera.rotation += Vector3(0, cameraLookSpeed * deltaTime, 0);
    }

    if (Input::keyDown("a"))
    {
        camera.rotation -= Vector3(0, cameraLookSpeed * deltaTime, 0);
    }

    // Calculate the forward direction based on the Y-axis rotation.
    Vector3 forward(
        -sin(camera.rotation.y),
        0,
        cos(camera.rotation.y)
    );

    // Normalize the forward vector if it isn't normalized already.
    forward = forward.normalized();

    if (Input::keyDown("w"))
    {
        camera.position -= forward * cameraMoveSpeed * deltaTime;
    }

    if (Input::keyDown("s"))
    {
        camera.position += forward * cameraMoveSpeed * deltaTime;
    }
} 