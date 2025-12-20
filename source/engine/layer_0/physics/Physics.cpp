#include "Log.h"
#include "Physics.h"
#include "Jolt.h"

PhysicsAPI Physics:: _currentType;
IPhysicsAPI* Physics::_currentAPI = nullptr;
Vector3 Physics::_gravity = Vector3(0,0,0);

string Physics::TypeName()
{
    switch (_currentType)
    {
        case ( PhysicsAPI::Jolt ):
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

void Physics::Initialize(PhysicsConfig desc)
{
    _gravity = desc.Gravity;

    switch (desc.PhysicsTypeAPI)
    {
        case ( PhysicsAPI::Jolt ):
        _currentAPI = new Jolt();
        break;
    }

    if (_currentAPI != nullptr)
    {
        _currentType = desc.PhysicsTypeAPI;
        _currentAPI->Initialize();
    }
    else
    {
        ENGINE_ERROR("Unsupported Physics API selected!");
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
        _currentType = PhysicsAPI::Null;
    }
}

void Physics::CreateBody(PhysCommand command)
{
    if (_currentAPI != nullptr)
    {
        _currentAPI->CreateBody(command);
    }
}

PhysResponse Physics::GetBodyData(uniqueID id)
{
    PhysResponse returnValue;

    if (_currentAPI != nullptr)
    {
        returnValue = _currentAPI->GetBodyData(id);
    }

    return returnValue;
}

