#pragma once

#include "Window.h"

class Engine
{
    public:
	bool Initialize();
	void UnInitialize();
	void Pause();
	void Resume();
	void Tick();
	bool IsRunning();

	private:
	bool m_isRunning = false;
	Window* m_window = nullptr;
};

