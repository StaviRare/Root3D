#pragma once

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
	bool isRunning;
};

