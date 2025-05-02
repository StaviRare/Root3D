#pragma once

#include <set>
#include <unordered_map>
#include "Types.h"
#include "Core.h"

class Entity;

class EntityPool
{
    friend Core;
    friend Entity;

    public:
    static Entity* GetEntityByID(uniqueID id);
    static const std::set<Entity*>& GetEntities();

    private:
    static uniqueID nextID;
    static std::set<Entity*> entities;
    static std::unordered_map<uniqueID, Entity*> entityMap;

    private:
    static void Tick();
    static uniqueID GenerateID();
    static uniqueID AddEntity(Entity* entity);
    static void RemoveEntity(Entity* entity);
};
