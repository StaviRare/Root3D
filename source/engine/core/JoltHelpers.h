#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

// All Jolt symbols are in the JPH namespace
using namespace JPH;

namespace Layers
{
    static constexpr ObjectLayer NON_MOVING = 0;
    static constexpr ObjectLayer MOVING = 1;
    static constexpr ObjectLayer NO_COLLISION = 2;
    static constexpr ObjectLayer NUM_LAYERS = 3;
}

class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
{
    public:
    bool ShouldCollide(ObjectLayer o1, ObjectLayer o2) const override;
};

class MyContactListener : public ContactListener
{
    public:
    ValidateResult OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult) override;
    void OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override;
    void OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override;
    void OnContactRemoved(const SubShapeIDPair& inSubShapePair) override;
};

class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
{
    public:
    BPLayerInterfaceImpl();
    uint GetNumBroadPhaseLayers() const override;
    BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer layer) const override;

    private:
    BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
{
    public:
    bool ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override;
};
