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

int main()
{
    Run();
    return 0;
}