#include "Core.h"

int main()
{
    Core::Initialize();

    // Main loop
    while (true)
    {
        Core::Tick();
    }

    Core::UnInitialize();

    return 0;
}