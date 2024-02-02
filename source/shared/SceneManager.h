#pragma once
#include <map>
#include <functional>
#include "Scene.h"

class SceneManager
{
public:
    static void registerScene(int index, std::function<Scene* ()> constructor);
    static void loadScene(int index);
    static void runScene();

private:
    static Scene* currentScene;
    static std::map<int, std::function<Scene* ()>>& getSceneRegistry();
};