#include "Entitypool.h"
#include "Entity.h"

std::set<Entity*> EntityPool::entities;

void EntityPool::AddEntity(Entity* entity)
{
    entities.insert(entity);
}

void EntityPool::RemoveEntity(Entity* entity)
{
    entities.erase(entity);
}

const std::set<Entity*>& EntityPool::GetEntities()
{
    return entities;
}
