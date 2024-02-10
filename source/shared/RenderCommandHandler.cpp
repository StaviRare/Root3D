#include "RenderCommandHandler.h"
#include "RenderQueue.h"
#include "EntityPool.h"
#include "MeshData.h"
#include "Mesh.h"
#include "Entity.h"
#include "RenderCommand.h"
#include "Renderer.h"
#include "Debug.h"
#include "Matrix4.h"
#include "Camera.h"

void RenderCommandHandler::Tick()
{
    const std::set<Entity*>& entities = EntityPool::GetEntities();

    if (Camera::Exists())
    {
        Camera& camera = Camera::GetInstance();

        Vector3 cameraUp(0.0f, 1.0f, 0.0f);
        Vector3 cameraTarget = camera.transform.position + camera.transform.getForward();
        Matrix4 view = Matrix4::LookAt(camera.transform.position, cameraTarget, cameraUp);
        Matrix4 projection = Matrix4::Perspective(camera.fov, camera.GetAspect(), camera.nearClipPlane, camera.farClipPlane);

        // ToDo - Remove this hard coded directional light. Create object.
        Vector3 lightDir(1.0f, 0.0f, 0.0f);
        Vector3 lightColor(1.0f, 1.0f, 1.0f);
        lightDir.normalize();

        GlobalRenderCommand renderOnce;
        view.CopyToArray(renderOnce.viewMatrix);
        projection.CopyToArray(renderOnce.projectionMatrix);

        renderOnce.backgroundColor[0] = camera.backgroundColor.r;
        renderOnce.backgroundColor[1] = camera.backgroundColor.g;
        renderOnce.backgroundColor[2] = camera.backgroundColor.b;
        renderOnce.backgroundColor[3] = camera.backgroundColor.a;
        
        renderOnce.directionalLightDirection[0] = lightDir.x;
        renderOnce.directionalLightDirection[1] = lightDir.y;
        renderOnce.directionalLightDirection[2] = lightDir.z;

        renderOnce.directionalLightColor[0] = lightColor.x;
        renderOnce.directionalLightColor[1] = lightColor.y;
        renderOnce.directionalLightColor[2] = lightColor.z;

        RenderQueue::EnqueueOnce(renderOnce);

        for (const Entity* entity : entities)
        {
            Renderer* renderer = entity->GetComponent<Renderer>();
            MeshData* meshData = entity->GetComponent<MeshData>();

            if (renderer && meshData)
            {
                ObjectRenderCommand renderCommand;

                renderCommand.shader = &renderer->material.shader;
                renderCommand.texture = &renderer->material.texture;

                renderCommand.indices = meshData->mesh.GetIndices().data();
                renderCommand.indicesSize = meshData->mesh.GetIndices().size();

                // Convert Vector3/2 to floats: size x3 (Vector3), x2 (Vector2).
                renderCommand.vertices = reinterpret_cast<const float*>(meshData->mesh.GetVertices().data());
                renderCommand.verticesSize = meshData->mesh.GetVertices().size() * 3;

                renderCommand.texCoords = reinterpret_cast<const float*>(meshData->mesh.GetTexCoords().data());
                renderCommand.texCoordsSize = meshData->mesh.GetTexCoords().size() * 2;

                renderCommand.normals = reinterpret_cast<const float*>(meshData->mesh.GetNormals().data());
                renderCommand.normalsSize = meshData->mesh.GetNormals().size() * 3;

                Matrix4 model = Matrix4::Identity();
                model = model.Scale(entity->transform.scale);
                model = model.Rotate(entity->transform.eulerAngles);
                model = model.Translate(entity->transform.position);
                
                model.CopyToArray(renderCommand.modelMatrix);

                RenderQueue::Enqueue(renderCommand);
            }
        }
    }
}