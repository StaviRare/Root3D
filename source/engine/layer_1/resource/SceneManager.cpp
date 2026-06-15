#include "Log.h"
#include "SceneManager.h"

Scene* SceneManager::s_nextScene = nullptr;
Scene* SceneManager::s_currentScene = nullptr;
std::map<int, std::function<Scene*()>> SceneManager::s_sceneRegistry;

void SceneManager::LoadScene(int index)
{
    auto it = s_sceneRegistry.find(index);

    if (it != s_sceneRegistry.end())
    {
        Scene* newScene = it->second();

        // If first scene, don't queue
        if (s_currentScene == nullptr)
        {
            s_currentScene = newScene;
            s_currentScene->onLoad();
        }
        else
        {
            if (s_nextScene)
            {
                delete s_nextScene;
            }

            s_nextScene = newScene;
        }
    }
    else
    {
        ENGINE_ERROR("Scene " + std::to_string(index) + " not found.");
    }
}

void SceneManager::UnloadScene()
{
    if (s_currentScene)
    {
        s_currentScene->onUnload();
        delete s_currentScene;
        s_currentScene = nullptr;
    }
}

Scene* SceneManager::GetCurrentScene()
{
    return s_currentScene;
}

void SceneManager::RegisterScene(int index, std::function<Scene* ()> constructor)
{
    if (s_sceneRegistry.find(index) != s_sceneRegistry.end())
    {
        ENGINE_ERROR("Scene " + std::to_string(index) + " index already taken.");
        return;
    }

    s_sceneRegistry[index] = constructor;
}

void SceneManager::Initialize()
{
    // Nothing at the moment
}

void SceneManager::UnInitialize()
{
    UnloadScene();
}

void SceneManager::Tick()
{
    if (s_currentScene)
    {
        s_currentScene->Tick();
    }
}

void SceneManager::LateTick()
{
    if (s_currentScene)
    {
        s_currentScene->LateTick();
    }
}

void SceneManager::OnAppFocus(bool hasFocus)
{
    if (s_currentScene)
    {
        s_currentScene->OnAppFocus(hasFocus);
    }
}

void SceneManager::OnAppPause(bool isPaused)
{
    if (s_currentScene)
    {
        s_currentScene->OnAppPause(isPaused);
    }
}

void SceneManager::EndFrame()
{
    if (s_nextScene)
    {
        if (s_currentScene)
        {
            s_currentScene->onUnload();
            delete s_currentScene;
        }

        s_currentScene = s_nextScene;
        s_nextScene = nullptr;
        s_currentScene->onLoad();
    }
}