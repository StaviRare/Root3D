#pragma once

#include "App.h"
#include "Engine.h"

class Runtime
{
	public:
	void Initialize();
	void UnInitialize();
	void Pause();
	void Resume();
	void Tick();
	bool IsRunning();

	private:
	App* app = nullptr;
	Engine* engine = nullptr;
};