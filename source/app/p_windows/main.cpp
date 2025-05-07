#include <windows.h>
#include "EngineRuntime.h"

void Run()
{
    EngineRuntime::Initialize();

    // Main loop
    while (true)
    {
        EngineRuntime::Tick();
    }

    EngineRuntime::UnInitialize();
}

#ifdef DEBUG
int main()
{
    Run();
    return 0;
}
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Run();
    return 0;
}
#endif
