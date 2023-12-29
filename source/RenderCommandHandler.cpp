#include "RenderCommandHandler.h"
#include "RenderQueue.h"
#include "EntityPool.h"
#include "MeshData.h"
#include "Mesh.h"
#include "Entity.h"
#include "RenderCommand.h"
#include "Renderer.h"
#include "Debug.h"

void RenderCommandHandler::Tick()
{
    const std::set<Entity*>& entities = EntityPool::GetEntities();

    for (const Entity* entity : entities)
    {
        Renderer* renderer = entity->GetComponent<Renderer>();
        MeshData* meshData = entity->GetComponent<MeshData>();

        if (renderer && meshData)
        {
            RenderCommand renderCommand;
            renderCommand.mesh = &meshData->mesh;
            renderCommand.texture = &renderer->material.texture;
            renderCommand.position = entity->transform.position;
            renderCommand.eulerAngles = entity->transform.eulerAngles;
            renderCommand.scale = entity->transform.scale;
            renderCommand.shader = &renderer->material.shader;

            RenderQueue::Enqueue(renderCommand);
        }
    }
}