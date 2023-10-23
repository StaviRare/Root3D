#pragma once

#include <set>

class Entity;  // Forward declaration 

class EntityPool
{
public:
    static void AddEntity(Entity* entity);
    static void RemoveEntity(Entity* entity);
    static const std::set<Entity*>& GetEntities();

private:
    static std::set<Entity*> entities;
};
