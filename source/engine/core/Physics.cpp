#include "Debug.h"
#include "Physics.h"
#include "Config.h"
#include "Jolt.h"

PhysicsType Physics:: _currentType;
PhysicsAPI* Physics::_currentAPI = nullptr;
Vector3 Physics::_gravity = Vector3(0,0,0);

string Physics::TypeName()
{
    switch (_currentType)
    {
        case ( PhysicsType::Jolt ):
        return "Jolt";

        default:
        return "Null";
    }
}

Vector3 Physics::GetGravity()
{
    return _gravity;
}

void Physics::SetGravity(Vector3 gravity)
{
    _gravity = gravity;
}

void Physics::Initialize()
{
    RuntimeSettings config = Config::Runtime();
    PhysicsType type = config.PhysicsTypeAPI;
    _gravity = config.Gravity;

    switch (type)
    {
        case ( PhysicsType::Jolt ):
        _currentAPI = new Jolt();
        break;
    }

    if (_currentAPI != nullptr)
    {
        _currentType = type;
        _currentAPI->Initialize();
    }
    else
    {
        Debug::LogError("Unsupported Physics API selected!");
    }
}

void Physics::Simulate()
{
    if (_currentAPI != nullptr)
    {
        _currentAPI->Simulate();
    }
}

void Physics::UnInitialize()
{
    if (_currentAPI != nullptr)
    {
        _currentAPI->UnInitialize();

        delete _currentAPI;
        _currentAPI = nullptr;
        _currentType = PhysicsType::Null;
    }
}
