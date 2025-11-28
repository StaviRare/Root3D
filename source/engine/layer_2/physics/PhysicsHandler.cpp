#include "PhysicsHandler.h"
#include "Entity.h"
#include "Log.h"
#include "Physics.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "MeshCollider.h"
#include "SphereCollider.h"
#include "MeshFilter.h"
#include "SceneManager.h"

// Colliders and rigidbodies will self-register here upon creation, eliminating the need to loop through entities.
// Colliders and rigidbodies will self-register here upon creation, eliminating the need to loop through entities.

void PhysicsHandler::SetData()
{
    Scene* currentScene = SceneManager::GetCurrentScene();

    if (currentScene == nullptr)
    {
        return;
    }

    const std::set<Entity*>& entities = currentScene->GetEntities();
    // ToDo - Check if scale has changed -> update the scale.


    if (entities.empty())
    {
        return;
    }

	for (Entity* entity : entities)
	{
        RigidBody* rigidBody = entity->GetComponent<RigidBody>();

        if (rigidBody)
        {
            PhysCommand command;
            command.entityID = entity->GetId();

            PhysTransform transform;
            transform.scale[0] = entity->transform.scale.x;
            transform.scale[1] = entity->transform.scale.y;
            transform.scale[2] = entity->transform.scale.z;
            transform.position[0] = entity->transform.position.x;
            transform.position[1] = entity->transform.position.y;
            transform.position[2] = entity->transform.position.z;
            transform.rotation[0] = entity->transform.rotation.x;
            transform.rotation[1] = entity->transform.rotation.y;
            transform.rotation[2] = entity->transform.rotation.z;
            transform.rotation[3] = entity->transform.rotation.w;

            PhysRigidbody rigidbody;
            rigidbody.isStatic = rigidBody->IsStatic;
            rigidbody.mass = rigidBody->Mass;
            rigidbody.drag = rigidBody->Drag;
            rigidbody.velocity[0] = rigidBody->Velocity.x;
            rigidbody.velocity[1] = rigidBody->Velocity.y;
            rigidbody.velocity[2] = rigidBody->Velocity.z;
            rigidbody.angularDrag = rigidBody->AngularDrag;
            rigidbody.angularVelocity[0] = rigidBody->AngularVelocity.x;
            rigidbody.angularVelocity[1] = rigidBody->AngularVelocity.y;
            rigidbody.angularVelocity[2] = rigidBody->AngularVelocity.z;

            PhysCollider collider;
            BoxCollider* boxCollider = entity->GetComponent<BoxCollider>();
            SphereCollider* sphereCollider = entity->GetComponent<SphereCollider>();
            MeshCollider* meshCollider = entity->GetComponent<MeshCollider>();

            if (boxCollider)
            {
                collider.type = 1;
                collider.boxSize[0] = boxCollider->size.x;
                collider.boxSize[1] = boxCollider->size.y;
                collider.boxSize[2] = boxCollider->size.z;
            }
            else if (sphereCollider)
            {
                collider.type = 2;
                collider.sphereRadius = sphereCollider->radius;
            }
            else if (meshCollider)
            {
                MeshFilter* meshFilter = entity->GetComponent<MeshFilter>();

                if (meshFilter)
                {
                    collider.type = 3;
                    collider.vertices = reinterpret_cast<const float*>( meshFilter->mesh.GetVertices().data() );
                    collider.verticesSize = meshFilter->mesh.GetVertices().size() * 3;
                }
            }
            else
            {
                collider.type = 0;
            }

            command.collider = collider;
            command.transform = transform;
            command.rigidbody = rigidbody;

            Physics::CreateBody(command);
        }
	}
}

void PhysicsHandler::GetData()
{
    Scene* currentScene = SceneManager::GetCurrentScene();

    if (currentScene == nullptr)
    {
        return;
    }

    const std::set<Entity*>& entities = currentScene->GetEntities();

    if (entities.empty())
    {
        return;
    }

	for (Entity* entity : entities)
	{
		PhysResponse response = Physics::GetBodyData(entity->GetId());

        if (response.hasData)
        {
            entity->transform.position = Vector3(response.position[0], response.position[1], response.position[2]);
            entity->transform.rotation = Quaternion(response.rotation[0], response.rotation[1], response.rotation[2], response.rotation[3]);
        }
	}
}

