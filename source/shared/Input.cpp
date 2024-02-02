#include "Input.h"
#include "Platform.h"
#include "PlatformDetector.h"
#include "DesktopInput.h"

IPlatformInput* Input::inputHandler = nullptr;

void Input::Initialize()
{
    Platform platform = PlatformDetector::GetPlatform();

    switch (platform)
    {
        case Platform::Android:
        break;

        case Platform::Windows:
        inputHandler = new DesktopInput();
        break;
    }

    if (inputHandler)
    {
        inputHandler->Initialize();
    }
}

void Input::Tick()
{
    if (inputHandler)
    {
        inputHandler->Tick();
    }
}

void Input::UnInitialize()
{
    if (inputHandler)
    {
        inputHandler->UnInitialize();
    }
}

bool Input::GetKey(const string& key)
{
    bool returnValue = false;

    if (inputHandler)
    {
        returnValue = inputHandler->GetKey(key);
    }

    return returnValue;
}

bool Input::GetKeyDown(const string& key)
{
    bool returnValue = false;

    if (inputHandler)
    {
        returnValue = inputHandler->GetKeyDown(key);
    }

    return returnValue;
}

bool Input::GetKeyUp(const string& key)
{
    bool returnValue = false;

    if (inputHandler)
    {
        returnValue = inputHandler->GetKeyUp(key);
    }

    return returnValue;
}