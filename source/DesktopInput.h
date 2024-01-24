#pragma once

#include <array>
#include "IPlatformInput.h"

class DesktopInput : public IPlatformInput
{
	public:
	void Initialize() override;
	void Tick() override;
	void UnInitialize() override;
	bool GetKey(const string& key) override;
	bool GetKeyDown(const string& key) override;
	bool GetKeyUp(const string& key) override;

	private:
	std::array<bool, 256> keyState;
	std::array<bool, 256> keyStateLastFrame;
};