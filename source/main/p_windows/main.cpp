#include <windows.h>
#include "Runtime.h"

void Run()
{
    auto runtime = new Runtime();
    runtime->Initialize();

    // Main loop
    while (runtime->IsRunning())
    {
        runtime->Tick();
    }

    runtime->UnInitialize();
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
