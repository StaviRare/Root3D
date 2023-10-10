#include "GraphicsWrapper.h"
#include "Debug.h"
#include "Timer.h"
#include "Camera.h"

#include "ExampleScene.h"
#include "SceneManager.h"


// Remove this
Vector3 cameraPosition = Vector3(0.0f, 0.0f, 3.0f);
float cameraYaw = 0.0f;
float cameraMoveSpeed = 3.00f;
float cameraLookSpeed = 2.00f;
float cameraFOV = 60.0f;


int main()
{
    // Initialize
    Timer::initialize();
    GraphicsWrapper::Initialize(APIType::OpenGL);


    SceneManager::loadScene(0);

    // Main loop
    while (true) {
        GraphicsWrapper::ClearScreen();
        GraphicsWrapper::ExecuteRenderCommands();
        //GraphicsWrapper::SwapBuffers();

        SceneManager::runScene();


        Timer::calculateLoopTime();
    }

    return 0;
}