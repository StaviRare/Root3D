#include <Windows.h>

#include "DesktopInput.h"
#include "Debug.h"

void DesktopInput::Initialize()
{
	for (int i = 0; i < 256; ++i)
	{
		keyState[i] = false;
		keyStateLastFrame[i] = false;
	}
}

void DesktopInput::Tick()
{
	keyStateLastFrame = keyState;

	for (int i = 0; i < 256; ++i)
	{
		keyState[i] = ( GetAsyncKeyState(i) & 0x8000 ) != 0;
	}
}

void DesktopInput::UnInitialize()
{
	// Nothing at the moment
}

bool DesktopInput::GetKey(const string& key)
{
	if (key.length() != 1)
	{
		return false;
	}

	int vk = VkKeyScan(key[0]);

	return keyState[vk];
}

bool DesktopInput::GetKeyDown(const string& key)
{
	if (key.length() != 1)
	{
		return false;
	}

	int vk = VkKeyScan(key[0]);

	return keyState[vk] && !keyStateLastFrame[vk];
}

bool DesktopInput::GetKeyUp(const string& key)
{
	if (key.length() != 1)
	{
		return false;
	}

	int vk = VkKeyScan(key[0]);

	return !keyState[vk] && keyStateLastFrame[vk];
}