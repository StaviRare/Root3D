#include <array>
#include <Windows.h>
#include "Input.h"

static std::array<bool, 256> keyState;
static std::array<bool, 256> keyStateLastFrame;

static int ResolveVirtualKey(const std::string& key)
{
	int result = -1;
	bool validLength = (key.size() == 1);
	bool isAscii = validLength && static_cast<unsigned char>(key[0]) <= 127;

	if (isAscii)
	{
		int vk = VkKeyScan(static_cast<unsigned char>(key[0]));
		if (vk != -1)
		{
			result = vk & 0xFF;
		}
	}

	return result;
}

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

bool Input::GetKey(const std::string& key)
{
	int vk = ResolveVirtualKey(key);
	return vk >= 0 && keyState[vk];
}

bool Input::GetKeyDown(const std::string& key)
{
	int vk = ResolveVirtualKey(key);
	return vk >= 0 && keyState[vk] && !keyStateLastFrame[vk];
}

bool Input::GetKeyUp(const std::string& key)
{
	int vk = ResolveVirtualKey(key);
	return vk >= 0 && !keyState[vk] && keyStateLastFrame[vk];
}