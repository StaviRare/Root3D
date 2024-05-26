#include "EntityPool.h"
#include "Entity.h"

uniqueID EntityPool::nextID = 1;
std::set<Entity*> EntityPool::entities;
std::unordered_map<uniqueID, Entity*> EntityPool::entityMap;

void EntityPool::AddEntity(Entity* entity)
{
    entity->ID = GenerateID();
    entities.insert(entity);
    entityMap[entity->ID] = entity;
}

void EntityPool::RemoveEntity(Entity* entity)
{
    entities.erase(entity);
    entityMap.erase(entity->ID);
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