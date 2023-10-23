#pragma once

#include <vector>
#include <type_traits>
#include <typeinfo>

#include "Transform.h"
#include "Component.h"
#include "Entitypool.h"

class Entity
{
public:
    Transform transform;
    std::vector<Component*> components;

    template <typename T>
    T& AddComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must be a subclass of Component");

        for (Component* existingComponent : components)
        {
            if (dynamic_cast<T*>(existingComponent) != nullptr)
            {
                return *dynamic_cast<T*>(existingComponent);
            }
        }

        T* component = new T();
        components.push_back(component);
        return *component;
    }

    template <typename T>
    T* GetComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must be a subclass of Component");

        for (Component* existingComponent : components)
        {
            if (typeid(*existingComponent) == typeid(T))
            {
                return dynamic_cast<T*>(existingComponent);
            }
        }
        return nullptr;
    }

    template <typename T>
    bool RemoveComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must be a subclass of Component");

        for (size_t i = 0; i < components.size(); ++i)
        {
            if (typeid(*components[i]) == typeid(T))
            {
                delete components[i];
                components.erase(components.begin() + i);
                return true;
            }
        }
        return false;
    }

    Entity()
    {
        EntityPool::AddEntity(this);
    }

    ~Entity()
    {
        EntityPool::RemoveEntity(this);

        for (Component* component : components)
        {
            delete component;
        }
    }
};