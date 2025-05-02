#include "EntityPool.h"
#include "Entity.h"

uniqueID EntityPool::nextID = 1;
std::set<Entity*> EntityPool::entities;
std::unordered_map<uniqueID, Entity*> EntityPool::entityMap;

void EntityPool::Tick()
{
    for (Entity* entity : entities)
    {
        entity->Tick();
    }
}

uniqueID EntityPool::AddEntity(Entity* entity)
{
    auto ID = GenerateID();
    entities.insert(entity);
    entityMap[ID] = entity;

    return ID;
}

void EntityPool::RemoveEntity(Entity* entity)
{
    entities.erase(entity);
}

Entity* EntityPool::GetEntityByID(uniqueID id)
{
    if (entityMap.find(id) != entityMap.end())
    {
        return entityMap[id];
    }

    return nullptr;
}

const std::set<Entity*>& EntityPool::GetEntities()
{
    return entities;
}

uniqueID EntityPool::GenerateID()
{
    return nextID++;
}