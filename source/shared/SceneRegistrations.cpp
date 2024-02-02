#include "SceneManager.h"
#include "ExampleScene.h"

struct SceneRegistrar
{
    SceneRegistrar()
    {
        // Register all scenes here
        SceneManager::registerScene(0, [] { return new ExampleScene(); });
    }
};

// Static instance for auto-registration of scenes.
// It triggers the constructor of SceneRegistrar, 
// which registers all scenes with the SceneManager,
// ensuring this happens before main() starts.
static SceneRegistrar sceneRegistrar;