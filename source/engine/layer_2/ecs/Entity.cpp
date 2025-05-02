#include "Entity.h"
#include "Entitypool.h"

Entity::Entity()
{
    ID = EntityPool::AddEntity(this);
}

Entity::~Entity()
{
    EntityPool::RemoveEntity(this);

    for (Component* comp : components)
    {
        comp->OnDestroy();
        delete comp;
    }
}

void Entity::Tick()
{
    for (Component* c : components)
    {
        c->Tick();
    }
}

uniqueID Entity::GetID() const
{
    return ID;
}
