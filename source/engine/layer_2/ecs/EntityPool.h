#pragma once

#include <set>
#include <unordered_map>
#include "Types.h"

class Entity;

class EntityPool
{
    private:
    static uniqueID nextID;
    static std::set<Entity*> entities;
    static std::unordered_map<uniqueID, Entity*> entityMap;

    public:
    static void AddEntity(Entity* entity);
    static void RemoveEntity(Entity* entity);
    static Entity* GetEntityByID(uniqueID id);
    static const std::set<Entity*>& GetEntities();

    private:
    static uniqueID GenerateID();
};
