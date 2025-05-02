#pragma once

#include <map>
#include <functional>
#include "Scene.h"

class SceneManager
{
    public:
    static void LoadScene(int index);
    static void UnloadScene();
    static void RegisterScene(int index, std::function<Scene* ( )> constructor);

    private:
    static Scene* currentScene;
    static std::map<int, std::function<Scene* ( )>>& getSceneRegistry();
};