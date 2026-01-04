#pragma once

#include "App.h"
#include "Engine.h"

class Runtime
{
	public:
	void Initialize();
	void UnInitialize();
	void Pause(); // not sure we need it.
	void Resume(); // not sure we need it.
	void Tick();
	bool IsRunning();

	private:
	App* app = nullptr;
	Engine* engine = nullptr;
};