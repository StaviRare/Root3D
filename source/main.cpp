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
    // Maybe entity manager here.
    SceneManager::loadScene(0);
}

void Tick()
{
    // Maybe entity manager here.
    GraphicsWrapper::ClearScreen();
    GraphicsWrapper::ExecuteRenderCommands();
    //GraphicsWrapper::SwapBuffers();

    SceneManager::runScene();
    Timer::calculateLoopTime();
}