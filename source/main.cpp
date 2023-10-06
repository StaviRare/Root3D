#include "GraphicsWrapper.h"
#include "Debug.h"
#include "Timer.h"

// ToDo:
// Look into creating object with components. (transform, material..)


int main()
{
    // Initialize
    Timer::initialize();
    GraphicsWrapper::Initialize(APIType::OpenGL);

    // Main loop
    while (true) {
        GraphicsWrapper::ClearScreen();
        GraphicsWrapper::ExecuteRenderCommands();
        //GraphicsWrapper::SwapBuffers();


        // Add camera. Move camera.

        Timer::calculateLoopTime();
    }

    return 0;
}