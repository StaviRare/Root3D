#include "ExampleScene.h"
#include "Camera.h"
#include "Timer.h"
#include "Input.h"
#include "Mesh.h"
#include "Entity.h"
#include "MeshData.h"
#include "MeshGenerator.h"
#include "Debug.h"

static Entity entity;
static Entity entity2;

static Camera camera;
static float cameraMoveSpeed = 3.00f;
static float cameraLookSpeed = 2.00f;

void initCamera();
void initCube();
void handleCameraMovement();
void handleCubeTransform();



void ExampleScene::init()
{
    initCamera();
    initCube();
}

void ExampleScene::tick()
{
    handleCameraMovement();
    handleCubeTransform();
}


void initCamera()
{
    camera.fov = 60.0f;
    camera.transform.position = Vector3(0, 0, 3);
}

void initCube()
{
    // cube 1
    entity.transform.position = Vector3(0, 0, 0);
    Mesh mesh = MeshGenerator::GetCube();
    MeshData* meshData = entity.AddComponent<MeshData>();
    meshData->mesh = mesh;

    // cube 2
    entity2.transform.position = Vector3(1.5f, 0, 0);
    Mesh mesh2 = MeshGenerator::GetCube();
    MeshData* meshData2 = entity2.AddComponent<MeshData>();
    meshData2->mesh = mesh2;
}

void handleCameraMovement()
{
    double deltaTime = Timer::getDeltaTime();

    if (Input::keyDown("d"))
    {
        camera.transform.rotation += Vector3(0, cameraLookSpeed * deltaTime, 0);
    }

    if (Input::keyDown("a"))
    {
        camera.transform.rotation -= Vector3(0, cameraLookSpeed * deltaTime, 0);
    }

    // Calculate the forward direction based on the Y-axis rotation.
    Vector3 forward(
        -sin(camera.transform.rotation.y),
        0,
        cos(camera.transform.rotation.y)
    );

    // Normalize the forward vector if it isn't normalized already.
    forward = forward.normalized();

    if (Input::keyDown("w"))
    {
        camera.transform.position -= forward * cameraMoveSpeed * deltaTime;
    }

    if (Input::keyDown("s"))
    {
        camera.transform.position += forward * cameraMoveSpeed * deltaTime;
    }
} 

void handleCubeTransform()
{
    entity.transform.rotation += Vector3(1, 1, 0);
}