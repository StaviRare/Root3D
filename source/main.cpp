#include "Debug.h"
#include "Timer.h"
#include "Camera.h"
#include "SceneManager.h"
#include "GraphicsWrapper.h"

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
    Timer::initialize();
    GraphicsWrapper::Initialize(APIType::OpenGL);
    SceneManager::loadScene(0);
}

void Tick()
{
    GraphicsWrapper::ClearScreen();
    GraphicsWrapper::ExecuteRenderCommands();
    //GraphicsWrapper::SwapBuffers();

    SceneManager::runScene();
    Timer::calculateLoopTime();
}