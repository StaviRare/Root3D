#include "Component.h"
#include "Entity.h"

void Component::SetOwner(Entity& e)
{
    owner = &e;
}

Entity& Component::GetEntity()
{
    return *owner;
}

Transform& Component::GetTransform()
{
    return owner->transform;
}