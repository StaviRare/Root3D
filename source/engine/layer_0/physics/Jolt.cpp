#include <unordered_map>
#include <set>

#include "Log.h"
#include "Jolt.h"
#include "Physics.h"
#include "EntityPool.h"
#include "RigidBody.h"
#include "Entity.h"
#include "Timer.h"
#include "JoltHelpers.h"
#include "MeshData.h"
#include "Collider.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "MeshCollider.h"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>

static TempAllocatorImpl* m_Allocator = nullptr;
static JobSystemThreadPool* m_JobSystem = nullptr;
static PhysicsSystem* m_PhysicsSystem = nullptr;

static BPLayerInterfaceImpl broadPhaseLayerInterface;
static ObjectVsBroadPhaseLayerFilterImpl objectVsBroadPhaseLayerFilter;
static ObjectLayerPairFilterImpl objectVsObjectLayerFilter;
static std::unordered_map<uint64_t, BodyID> entityBodyMap;

RefConst<Shape> CreateShape(Entity* entity)
{
    RefConst<Shape> returnValue = nullptr;
    Collider* collider = entity->GetComponent<Collider>();
    
    if (collider)
    {
        // Scale collider dimensions to fit primitive mesh sizes.
        const float scaleFactor = 0.5f;

        BoxCollider* boxCollider = entity->GetComponent<BoxCollider>();
        SphereCollider* sphereCollider = entity->GetComponent<SphereCollider>();
        MeshCollider* meshCollider = entity->GetComponent<MeshCollider>();

        if (boxCollider)
        {
            Vector3 finalScale = boxCollider->size * entity->transform.scale;
            returnValue = new BoxShape(Vec3(finalScale.x, finalScale.y, finalScale.z) * scaleFactor);
        }
        else if (sphereCollider)
        {
            returnValue = new SphereShape(sphereCollider->radius * scaleFactor);
        }
        else if (meshCollider)
        {
            MeshData* meshData = entity->GetComponent<MeshData>();

            if (meshData)
            {
                std::vector<Vec3> points;
                const std::vector<Vector3>& verts = meshData->mesh.GetVertices();
                points.reserve(verts.size());

                for (const auto& vert : verts)
                {
                    Vector3 scaledVert = vert * entity->transform.scale;
                    points.emplace_back(scaledVert.x, scaledVert.y, scaledVert.z);
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
            }
        }
    }

    if (returnValue == nullptr)
    {
        returnValue = new BoxShape(Vec3(0.5f, 0.5f, 0.5f));
    }

    return returnValue;
}

BodyID CreateBody(Entity* entity, BodyInterface& bodyInterface)
{
    BodyID returnValue;
    Collider* collider = entity->GetComponent<Collider>();
    RigidBody* rigidBody = entity->GetComponent<RigidBody>();
    
    if (rigidBody)
    {
        RefConst<Shape> shape = CreateShape(entity);

        if (shape)
        {
            Vec3 position(entity->transform.position.x, entity->transform.position.y, entity->transform.position.z);
            Quat rotation(entity->transform.rotation.x, entity->transform.rotation.y, entity->transform.rotation.z, entity->transform.rotation.w);

            ObjectLayer layer;

            if (collider)
            {
                layer = rigidBody->IsStatic ? Layers::NON_MOVING : Layers::MOVING;
            }
            else
            {
                layer = Layers::NO_COLLISION;
            }

            BodyCreationSettings bodyCreationSettings(
                shape,
                position,
                rotation,
                rigidBody->IsStatic ? EMotionType::Kinematic : EMotionType::Dynamic,
                layer
            );

            bodyCreationSettings.mRestitution = rigidBody->IsStatic ? 0.0f : 0.2f;

            Body* body = bodyInterface.CreateBody(bodyCreationSettings);

            if (body)
            {
                bodyInterface.AddBody(body->GetID(), rigidBody->IsStatic ? EActivation::DontActivate : EActivation::Activate);
                entityBodyMap[entity->GetID()] = body->GetID();
                returnValue = body->GetID();
            }
        }
    }

    return returnValue;
}

void Jolt::Initialize()
{
    RegisterDefaultAllocator();
    Factory::sInstance = new Factory();
    RegisterTypes();

    m_Allocator = new TempAllocatorImpl(10 * 1024 * 1024);
    m_JobSystem = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

    // This is world related. not jolt engine related.
    const uint cMaxBodies = 1024;
    const uint cNumBodyMutexes = 0;
    const uint cMaxBodyPairs = 1024;
    const uint cMaxContactConstraints = 1024;

    m_PhysicsSystem = new PhysicsSystem();
    m_PhysicsSystem->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
        broadPhaseLayerInterface, objectVsBroadPhaseLayerFilter, objectVsObjectLayerFilter);

    // Set from config
    Vector3 gravity = Physics::GetGravity();
    m_PhysicsSystem->SetGravity(Vec3(gravity.x, gravity.y, gravity.z));
}

void Jolt::Simulate()
{
    Vector3 currentGravity = Physics::GetGravity();
    m_PhysicsSystem->SetGravity(Vec3(currentGravity.x, currentGravity.y, currentGravity.z));

    TempAllocatorImpl tempAllocator(1024 * 1024); // 1 MB temporary allocator

    const std::set<Entity*>& entities = EntityPool::GetEntities();
    MyContactListener contactListener;
    m_PhysicsSystem->SetContactListener(&contactListener);

    BodyInterface& bodyInterface = m_PhysicsSystem->GetBodyInterface();

    for (Entity* entity : entities)
    {
        RigidBody* rigidBody = entity->GetComponent<RigidBody>();
        
        if (rigidBody)
        {
            BodyID bodyID;
            bool bodyExists = entityBodyMap.find(entity->GetID()) != entityBodyMap.end();

            if (bodyExists)
            {
                bodyID = entityBodyMap[entity->GetID()];
            }
            else
            {
                bodyID = CreateBody(entity, bodyInterface);
            }

            Vec3 position(entity->transform.position.x, entity->transform.position.y, entity->transform.position.z);
            Quat rotation(entity->transform.rotation.x, entity->transform.rotation.y, entity->transform.rotation.z, entity->transform.rotation.w);
            bodyInterface.SetPositionAndRotation(bodyID, position, rotation, EActivation::DontActivate);

            // ToDo - Check if scale has changed -> update the scale.
        }
    }

    float fixedDeltaTime = Timer::FixedDeltaTime();
    m_PhysicsSystem->Update(fixedDeltaTime, 1, &tempAllocator, m_JobSystem);

    for (const auto& [entityID, bodyID] : entityBodyMap)
    {
        Entity* entity = EntityPool::GetEntityByID(entityID);

        if (entity)
        {
            Vec3 updatedPosition = bodyInterface.GetPosition(bodyID);
            Quat updatedRotation = bodyInterface.GetRotation(bodyID);

            entity->transform.position = Vector3(updatedPosition.GetX(), updatedPosition.GetY(), updatedPosition.GetZ());
            entity->transform.rotation = Quaternion(updatedRotation.GetX(), updatedRotation.GetY(), updatedRotation.GetZ(), updatedRotation.GetW());
        }
    }
}

void Jolt::UnInitialize()
{
    for (auto& [entityID, bodyID] : entityBodyMap)
    {
        BodyInterface& bodyInterface = m_PhysicsSystem->GetBodyInterface();
        bodyInterface.RemoveBody(bodyID);
    }

    entityBodyMap.clear();

    delete m_PhysicsSystem;
    delete m_JobSystem;
    delete Factory::sInstance;
    delete m_Allocator;
}