#include "Debug.h"
#include "Time.h"
#include "Camera.h"
#include "SceneManager.h"
#include "GraphicsWrapper.h"
#include "PlatformDetector.h"
#include "RenderCommandHandler.h"

void Initialize();
void Tick();

int main()
{
    Initialize();

    // Main loop
    while (true)
    {
        Tick();
    }

    return 0;
}

void Initialize()
{
    PlatformDetector::Initialize();
    Time::Initialize();
    GraphicsWrapper::Initialize(APIType::OpenGL); // hard coded. will use config in future.
    SceneManager::loadScene(0);
}

void Tick()
{
    // Physics Simulation

    // Input Handling

    // Game Logic Processing
    SceneManager::runScene();

    // Scene Rendering
    RenderCommandHandler::Tick();
    GraphicsWrapper::ClearScreen();
    GraphicsWrapper::ExecuteRenderCommands();

    // UI Rendering

    // End of Frame Tasks ()
    //GraphicsWrapper::SwapBuffers();

    // Game Pausing

    // Decommissioning

    // Calculate loop time
    Time::CalculateLoopTime();
}