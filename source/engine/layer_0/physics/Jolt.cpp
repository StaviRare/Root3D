#include <set>
#include <unordered_map>

#include "Log.h"
#include "Jolt.h"
#include "Physics.h"
#include "Timer.h"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/RegisterTypes.h>

static PhysicsSystem* _physicsSystem = nullptr;
static TempAllocatorImpl* _allocator = nullptr;
static JobSystemThreadPool* _jobSystem = nullptr;
static BPLayerInterfaceImpl _broadPhaseLayerInterface;
static ObjectVsBroadPhaseLayerFilterImpl _objectVsBroadPhaseLayerFilter;
static ObjectLayerPairFilterImpl _objectVsObjectLayerFilter;
static std::unordered_map<uint64_t, BodyID> _entityBodyMap;
static const float _scaleFactor = 0.5f;

// Forward declarations of private functions
RefConst<Shape> CreateShape(PhysCommand command);
RefConst<Shape> CreateDefaultShape();
RefConst<Shape> CreateBoxShape(PhysCollider collider, PhysTransform transform);
RefConst<Shape> CreateSphereShape(PhysCollider collider);
RefConst<Shape> CreateMeshShape(PhysCollider collider, PhysTransform transform);

void Jolt::Initialize()
{
    RegisterDefaultAllocator();
    Factory::sInstance = new Factory();
    RegisterTypes();

    _allocator = new TempAllocatorImpl(10 * 1024 * 1024);
    _jobSystem = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

    // This is world related. not jolt engine related.
    const uint cMaxBodies = 1024;
    const uint cNumBodyMutexes = 0;
    const uint cMaxBodyPairs = 1024;
    const uint cMaxContactConstraints = 1024;

    _physicsSystem = new PhysicsSystem();
    _physicsSystem->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
        _broadPhaseLayerInterface, _objectVsBroadPhaseLayerFilter, _objectVsObjectLayerFilter);

    // Set from config
    Vector3 gravity = Physics::GetGravity();
    _physicsSystem->SetGravity(Vec3(gravity.x, gravity.y, gravity.z));
}

void Jolt::Simulate()
{
    Vector3 currentGravity = Physics::GetGravity();
    _physicsSystem->SetGravity(Vec3(currentGravity.x, currentGravity.y, currentGravity.z));

    TempAllocatorImpl tempAllocator(1024 * 1024); // 1 MB temporary allocator

    float fixedDeltaTime = Timer::FixedDeltaTime();
    _physicsSystem->Update(fixedDeltaTime, 1, &tempAllocator, _jobSystem);
}

void Jolt::UnInitialize()
{
    for (auto& [entityID, bodyID] : _entityBodyMap)
    {
        BodyInterface& bodyInterface = _physicsSystem->GetBodyInterface(); //put up
        bodyInterface.RemoveBody(bodyID);
    }

    _entityBodyMap.clear();

    delete _physicsSystem;
    delete _jobSystem;
    delete Factory::sInstance;
    delete _allocator;
}

void Jolt::CreateBody(PhysCommand command)
{
    bool bodyNotExists = _entityBodyMap.find(command.entityID) == _entityBodyMap.end();

    if (bodyNotExists)
    {
        RefConst<Shape> shape = CreateShape(command);

        if (!shape)
        {
            ENGINE_ERROR("[Jolt] Could not create a shape!");
        }
        else
        {
            ObjectLayer layer;

            if (command.collider.type == 0)
            {
                layer = Layers::NO_COLLISION;
            }
            else
            {
                layer = command.rigidbody.isStatic ? Layers::NON_MOVING : Layers::MOVING;
            }

            Vec3 position(command.transform.position[0], command.transform.position[1], command.transform.position[2]);
            Quat rotation(command.transform.rotation[0], command.transform.rotation[1], command.transform.rotation[2], command.transform.rotation[3]);

            BodyCreationSettings bodySettings;
            bodySettings.SetShape(shape);
            bodySettings.mPosition = position;
            bodySettings.mRotation = rotation;
            bodySettings.mMotionType = command.rigidbody.isStatic ? EMotionType::Kinematic : EMotionType::Dynamic;
            bodySettings.mObjectLayer = layer;
            bodySettings.mRestitution = command.rigidbody.isStatic ? 0.0f : 0.2f;

            BodyInterface& bodyInterface = _physicsSystem->GetBodyInterface();
            Body* body = bodyInterface.CreateBody(bodySettings);

            if (body)
            {
                bodyInterface.AddBody(body->GetID(), command.rigidbody.isStatic ? EActivation::DontActivate : EActivation::Activate);
                _entityBodyMap[command.entityID] = body->GetID();
            }
        }
    }
}

PhysResponse Jolt::GetBodyData(uniqueID id)
{
    BodyID bodyID;
    PhysResponse returnValue;
    bool bodyExists = _entityBodyMap.find(id) != _entityBodyMap.end();

    if (bodyExists)
    {
        bodyID = _entityBodyMap[id];
        returnValue.hasData = true;

        BodyInterface& bodyInterface = _physicsSystem->GetBodyInterface();
        Vec3 updatedPosition = bodyInterface.GetPosition(bodyID);
        Quat updatedRotation = bodyInterface.GetRotation(bodyID);

        returnValue.position[0] = updatedPosition.GetX();
        returnValue.position[1] = updatedPosition.GetY();
        returnValue.position[2] = updatedPosition.GetZ();

        returnValue.rotation[0] = updatedRotation.GetX();
        returnValue.rotation[1] = updatedRotation.GetY();
        returnValue.rotation[2] = updatedRotation.GetZ();
        returnValue.rotation[3] = updatedRotation.GetW();
    }
    else
    {
        returnValue.hasData = false;
    }

    return returnValue;
}

RefConst<Shape> CreateShape(PhysCommand command)
{
    switch (command.collider.type)
    {
        case 1:
        return CreateBoxShape(command.collider, command.transform);

        case 2:
        return CreateSphereShape(command.collider);

        case 3:
        return CreateMeshShape(command.collider, command.transform);

        default:
        return CreateDefaultShape();
    }
}

RefConst<Shape> CreateDefaultShape()
{
    return new BoxShape(Vec3(0.5f, 0.5f, 0.5f));
}

RefConst<Shape> CreateBoxShape(PhysCollider collider, PhysTransform transform)
{
    return new BoxShape(Vec3(
        collider.boxSize[0] * transform.scale[0],
        collider.boxSize[1] * transform.scale[1],
        collider.boxSize[2] * transform.scale[2]
    ) * _scaleFactor);
}

RefConst<Shape> CreateSphereShape(PhysCollider collider)
{
    return new SphereShape(collider.sphereRadius * _scaleFactor);
}

RefConst<Shape> CreateMeshShape(PhysCollider collider, PhysTransform transform)
{
    RefConst<Shape> returnValue = nullptr;

    std::vector<Vec3> points;
    points.reserve(collider.verticesSize / 3);

    for (unsigned int i = 0; i < collider.verticesSize; i += 3)
    {
        float scaledX = collider.vertices[i] * transform.scale[0];
        float scaledY = collider.vertices[i + 1] * transform.scale[1];
        float scaledZ = collider.vertices[i + 2] * transform.scale[2];

        points.emplace_back(scaledX, scaledY, scaledZ);
    }

    ConvexHullShapeSettings settings(points.data(), points.size(), 0.01f);
    ShapeSettings::ShapeResult result = settings.Create();

    if (result.HasError())
    {
        ENGINE_ERROR("Error creating convex hull shape: " + string(result.GetError().c_str()));
    }
    else
    {
        returnValue = result.Get();
    }

    return returnValue;
}