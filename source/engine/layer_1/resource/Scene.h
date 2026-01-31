#pragma once

#include <set>
#include "Entity.h"

class Scene
{
    public:
    virtual void onLoad() = 0;
    virtual void onUnload() = 0;

    void Tick()
    {
        for (Entity* entity : entities)
        {
            if (entity)
            {
                entity->Tick();
            }
        }

    }

    void LateTick()
    {
        for (Entity* entity : entities)
        {
            if (entity)
            {
                entity->LateTick();
            }
        }
    }

    void OnAppFocus(bool hasFocus)
    {
        for (Entity* entity : entities)
        {
            if (entity)
            {
                entity->OnAppFocus(hasFocus);
            }
        }
    }

    void OnAppPause(bool isPaused)
    {
        for (Entity* entity : entities)
        {
            if (entity)
            {
                entity->OnAppPause(isPaused);
            }
        }
    }

    void AddEntity(Entity* entity)
    {
        if (entity)
        {
            entities.insert(entity);
        }
    }

    void RemoveEntity(Entity* entity)
    {
        if (entity)
        {
            auto it = entities.find(entity);
            if (it != entities.end())
            {
                entities.erase(it);
            }
        }
    }

    const std::set<Entity*>& GetEntities()
    {
        return entities;
    }

    private:
    std::set<Entity*> entities;
};
