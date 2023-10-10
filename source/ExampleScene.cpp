#include "ExampleScene.h"
#include <iostream>
#include "Camera.h"
#include "Timer.h"

Camera camera;
//float cameraMoveSpeed = 3.00f;
//float cameraLookSpeed = 2.00f;

void ExampleScene::init() {
    camera.fov = 60.0f;

}

void ExampleScene::tick() {

    double deltaTime = Timer::getDeltaTime();
    camera.position = Vector3(0, 0, 3);
    camera.rotation += Vector3(0, 1 * deltaTime, 0);
}
