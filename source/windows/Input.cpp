#include <array>
#include <Windows.h>
#include "Input.h"

static std::array<bool, 256> keyState;
static std::array<bool, 256> keyStateLastFrame;

void Input::Initialize()
{
	for (int i = 0; i < 256; ++i)
	{
		keyState[i] = false;
		keyStateLastFrame[i] = false;
	}
}

void Input::Tick()
{
	keyStateLastFrame = keyState;

	for (int i = 0; i < 256; ++i)
	{
		keyState[i] = ( GetAsyncKeyState(i) & 0x8000 ) != 0;
	}
}

void Input::UnInitialize()
{
	// Nothing at the moment
}

bool Input::GetKey(const string& key)
{
	if (key.length() != 1)
	{
		return false;
	}

	int vk = VkKeyScan(key[0]);

	return keyState[vk];
}

bool Input::GetKeyDown(const string& key)
{
	if (key.length() != 1)
	{
		return false;
	}

	int vk = VkKeyScan(key[0]);

	return keyState[vk] && !keyStateLastFrame[vk];
}

bool Input::GetKeyUp(const string& key)
{
	if (key.length() != 1)
	{
		return false;
	}

	int vk = VkKeyScan(key[0]);

	return !keyState[vk] && keyStateLastFrame[vk];
}