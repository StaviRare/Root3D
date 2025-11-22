#pragma once

#include <map>
#include <functional>
#include "Scene.h"
#include "Engine.h"

class SceneManager
{
    friend class Engine;

    public:
    static void LoadScene(int index);
    static void UnloadScene();
    static void RegisterScene(int index, std::function<Scene* ( )> constructor);
    static Scene* GetCurrentScene();
    
    private:
    static Scene* currentScene;
    static std::map<int, std::function<Scene* ( )>>& getSceneRegistry();

    private:
    static void Initialze();
    static void UnInitialize();
    static void Tick();
    static void LateTick();
};