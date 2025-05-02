#pragma once

#include <vector>
#include <type_traits>
#include <typeinfo>

#include "Transform.h"
#include "Component.h"
#include "Entitypool.h"
#include "Types.h"

class Entity
{
    friend EntityPool;

    public:
    Transform transform;

    public:
    Entity();
    ~Entity();
    uniqueID GetID() const;

    private:
    uniqueID ID = -1;
    std::vector<Component*> components;

    private:
    void Tick();

    public:
    template <typename T>
    T* AddComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must be a subclass of Component");

        T* returnValue = nullptr;

        for (Component* c : components)
        {
            if (typeid(*c) == typeid(T))
            {
                returnValue = static_cast<T*>(c);
                break;
            }
        }

        if (returnValue == nullptr)
        {
            returnValue = new T();
            returnValue->SetOwner(*this);
            components.push_back(returnValue);
            returnValue->OnCreate();
        }

        return returnValue;
    }

    template <typename T>
    T* GetComponent() const
    {
        static_assert(std::is_base_of<Component, T>::value, "T must be a subclass of Component");

        T* returnValue = nullptr;

        for (Component* c : components)
        {
            if (typeid(*c) == typeid(T))
            {
                returnValue = static_cast<T*>(c);
                break;
            }
        }

        return returnValue;
    }

    template <typename T>
    void RemoveComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must be a subclass of Component");

        for (size_t i = 0; i < components.size(); ++i)
        {
            if (typeid(*components[i]) == typeid(T))
            {
                components[i]->OnDestroy();
                delete components[i];
                components.erase(components.begin() + i);
                break;
            }
        }
    }
};