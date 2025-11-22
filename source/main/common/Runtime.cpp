#include "Runtime.h"

void Runtime::Initialize()
{
	engine = new Engine();
	bool success = engine->Initialize();

	if (success)
	{
		app = new App();
		app->Initialize();
	}
}

void Runtime::UnInitialize()
{
	if (app != nullptr)
	{
		app->UnInitialize();
		delete app;
		app = nullptr;
	}

	if (engine != nullptr)
	{
		engine->UnInitialize();
		delete engine;
		engine = nullptr;
	}
}

void Runtime::Resume()
{
	if (engine != nullptr)
	{
		engine->Resume();
	}
}

void Runtime::Pause()
{
	if (engine != nullptr)
	{
		engine->Pause();
	}
}

void Runtime::Tick()
{
	if (engine != nullptr)
	{
		engine->Tick();
	}
}

bool Runtime::IsRunning()
{
	bool returnValue = false;

	if (engine != nullptr)
	{
		returnValue = engine->IsRunning();
	}

	return returnValue;
}