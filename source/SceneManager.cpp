#include "SceneManager.h"

// Add .h for every scene..
#include "ExampleScene.h"


Scene* SceneManager::currentScene = nullptr;


void SceneManager::loadScene(int sceneId) {
    switch (sceneId) {
    
    case 0:
        setScene(new ExampleScene());
        break;

    default:

        break;
    }

    currentScene->init();
}

void SceneManager::runScene() {
    if (currentScene != nullptr) {
        currentScene->tick();
    }
}

void SceneManager::setScene(Scene* scene) {
    if (currentScene != nullptr) {
        delete currentScene;
    }
    currentScene = scene;
}
