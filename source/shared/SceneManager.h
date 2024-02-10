#pragma once

#include <map>
#include <functional>
#include "Scene.h"

class SceneManager
{
    friend class Core;

    public:
    static void LoadScene(int index);
    static void RegisterScene(int index, std::function<Scene* ( )> constructor);

    private:
    static Scene* currentScene;
    static std::map<int, std::function<Scene* ( )>>& getSceneRegistry();

    static void RunScene();
};