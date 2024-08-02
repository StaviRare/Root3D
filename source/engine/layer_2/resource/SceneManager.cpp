#include "Log.h"
#include "SceneManager.h"

Scene* SceneManager::currentScene = nullptr;

std::map<int, std::function<Scene* ()>>& SceneManager::getSceneRegistry()
{
    static std::map<int, std::function<Scene* ()>> sceneFactory;
    return sceneFactory;
}

void SceneManager::RegisterScene(int index, std::function<Scene* ()> constructor)
{
    auto& registry = getSceneRegistry();

    if (registry.find(index) != registry.end())
    {
        ENGINE_ERROR("Scene " + std::to_string(index) + " index already taken.");
    }

    registry[index] = constructor;
}

void SceneManager::LoadScene(int index)
{
    auto& registry = getSceneRegistry();
    auto it = registry.find(index);
    
    if (it != registry.end())
    {
        if (currentScene != nullptr)
        {
            delete currentScene;
        }

        currentScene = it->second();
        currentScene->init();
    }
}

void SceneManager::RunScene()
{
    if (currentScene != nullptr)
    {
        currentScene->tick();
    }
    else
    {
        ENGINE_ERROR("Current scene is null!");
    }
}
