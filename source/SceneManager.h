#pragma once

#include "Scene.h"

class SceneManager {
public:
    SceneManager() = delete;

    static void loadScene(int sceneId);
    static void runScene();

private:
    static Scene* currentScene;
    static void setScene(Scene* scene);
};
