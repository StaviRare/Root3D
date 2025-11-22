#include "App.h"
#include "SceneManager.h"
#include "ExampleScene.h"

void App::Initialize()
{
	SceneManager::RegisterScene(0, [] { return new ExampleScene(); });
	SceneManager::LoadScene(0);
}

void App::UnInitialize()
{
	SceneManager::UnloadScene();
}