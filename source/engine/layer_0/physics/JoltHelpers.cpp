#include "JoltHelpers.h"

bool ObjectLayerPairFilterImpl::ShouldCollide(ObjectLayer o1, ObjectLayer o2) const
{
    switch (o1)
    {
        case Layers::NON_MOVING:
        return o2 == Layers::MOVING;
        case Layers::MOVING:
        return o2 != Layers::NO_COLLISION;
        case Layers::NO_COLLISION:
        return false;
        default:
        return false;
    }
}

ValidateResult MyContactListener::OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult)
{
    return ValidateResult::AcceptAllContactsForThisBodyPair;
}

void MyContactListener::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) 
{
    //ioSettings.mCombinedFriction = 0.8f;
    //ioSettings.mCombinedRestitution = 0.6f;
}

void MyContactListener::OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) 
{

}

void MyContactListener::OnContactRemoved(const SubShapeIDPair& inSubShapePair) 
{

}

BPLayerInterfaceImpl::BPLayerInterfaceImpl()
{
    mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayer(Layers::NON_MOVING);
    mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayer(Layers::MOVING);
    mObjectToBroadPhase[Layers::NO_COLLISION] = BroadPhaseLayer(Layers::NO_COLLISION);
}

uint BPLayerInterfaceImpl::GetNumBroadPhaseLayers() const
{
    return Layers::NUM_LAYERS;
}

BroadPhaseLayer BPLayerInterfaceImpl::GetBroadPhaseLayer(ObjectLayer layer) const
{
    return mObjectToBroadPhase[layer];
}

bool ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const
{
    switch (inLayer1)
    {
        case Layers::NON_MOVING:
        return inLayer2 == BroadPhaseLayer(Layers::MOVING);
        case Layers::MOVING:
        return inLayer2 != BroadPhaseLayer(Layers::NO_COLLISION);
        case Layers::NO_COLLISION:
        return false;
        default:
        return false;
    }
}
