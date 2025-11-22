#pragma once
#include <vector>
#include "Transform.h"
#include "Component.h"
#include "Object.h"

class Entity : public Object
{
    friend class Scene;

    public:
    Transform transform;

    Entity();
    explicit Entity(const string& name);
    ~Entity();

    private:
    std::vector<Component*> components;

    void Tick();
    void LateTick();

    public:
    template <typename T>
    T* AddComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must be a subclass of Component");

        for (Component* c : components)
        {
            if (typeid(*c) == typeid(T))
            {
                return static_cast<T*>(c);
            }
        }

        T* newComp = new T();
        newComp->SetOwner(*this);
        newComp->OnCreate();
        components.push_back(newComp);
        return newComp;
    }

    template <typename T>
    T* GetComponent() const
    {
        static_assert(std::is_base_of<Component, T>::value, "T must be a subclass of Component");

        for (Component* c : components)
        {
            if (typeid(*c) == typeid(T))
            {
                return static_cast<T*>(c);
            }
        }

        return nullptr;
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
