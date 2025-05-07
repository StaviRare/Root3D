#include "Core.h"
#include <windows.h>

void Run()
{
    Core::Initialize();

    // Main loop
    while (true)
    {
        Core::Tick();
    }

    Core::UnInitialize();
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
