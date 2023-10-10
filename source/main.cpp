#include "GraphicsWrapper.h"
#include "Debug.h"
#include "Timer.h"
#include "Camera.h"




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

    // This needs to go to scene object:
    Camera camera;


    // Main loop
    while (true) {
        GraphicsWrapper::ClearScreen();
        GraphicsWrapper::ExecuteRenderCommands();
        //GraphicsWrapper::SwapBuffers();



        // This needs to go to scene object:
        double deltaTime = Timer::getDeltaTime();
        camera.position = Vector3(0, 0, 2);
        camera.rotation += Vector3(0, 1 * deltaTime, 0);
        camera.fov = 60.0f;
        //glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
        //float cameraYaw = 0.0f;
        //float cameraMoveSpeed = 3.00f;
        //float cameraLookSpeed = 2.00f;
        //float cameraFOV = 60.0f;







        Timer::calculateLoopTime();
    }

    return 0;
}