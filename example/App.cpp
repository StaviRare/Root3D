#include "App.h"
#include "SceneManager.h"
#include "SplashScene.h"
#include "ExampleScene.h"

void App::Initialize()
{
	SceneManager::RegisterScene(0, [] { return new SplashScene(); });
	SceneManager::RegisterScene(1, [] { return new ExampleScene(); });
	SceneManager::LoadScene(0);
}

void App::UnInitialize()
{
	SceneManager::UnloadScene();
}