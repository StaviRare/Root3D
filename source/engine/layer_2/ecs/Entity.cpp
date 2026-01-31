#include "Entity.h"
#include "SceneManager.h"

Entity::Entity() : Object()
{
    Scene* currentScene = SceneManager::GetCurrentScene();
    
    if (currentScene)
    {
        currentScene->AddEntity(this);
    }
}

Entity::Entity(const string& name) : Object(name)
{
    Scene* currentScene = SceneManager::GetCurrentScene();
    
    if (currentScene)
    {
        currentScene->AddEntity(this);
    }
}

Entity::~Entity()
{
    for (Component* comp : components)
    {
        comp->OnDestroy();
        delete comp;
    }

    Scene* currentScene = SceneManager::GetCurrentScene();
    
    if (currentScene)
    {
        currentScene->RemoveEntity(this);
    }
}

void Entity::Tick()
{
    for (Component* c : components)
    {
        c->Tick();
    }
}

void Entity::LateTick()
{
    for (Component* c : components)
    {
        c->LateTick();
    }
}

void Entity::OnAppFocus(bool hasFocus)
{
    for (Component* c : components)
    {
        c->OnAppFocus(hasFocus);
    }
}

void Entity::OnAppPause(bool isPaused)
{
    for (Component* c : components)
    {
        c->OnAppPause(isPaused);
    }
}