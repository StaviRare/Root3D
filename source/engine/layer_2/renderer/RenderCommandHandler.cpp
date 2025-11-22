#include "RenderCommandHandler.h"
#include "RenderQueue.h"
#include "MeshData.h"
#include "Mesh.h"
#include "Entity.h"
#include "RenderCommand.h"
#include "Renderer.h"
#include "Debug.h"
#include "Matrix4.h"
#include "Camera.h"
#include "Light.h"
#include "SceneManager.h"

void RenderCommandHandler::Tick()
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

    if (Camera::Exists())
    {
        Camera& camera = Camera::GetInstance();

        Vector3 cameraUp(0.0f, 1.0f, 0.0f);
        Vector3 cameraTarget = camera.GetTransform().position + camera.GetTransform().getForward();
        Matrix4 view = Matrix4::LookAt(camera.GetTransform().position, cameraTarget, cameraUp);
        Matrix4 projection = Matrix4::Perspective(camera.fov, camera.GetAspect(), camera.nearClipPlane, camera.farClipPlane);

        GlobalRenderCommand renderOnce;
        view.CopyToArray(renderOnce.viewMatrix);
        projection.CopyToArray(renderOnce.projectionMatrix);

        renderOnce.backgroundColor[0] = camera.backgroundColor.r;
        renderOnce.backgroundColor[1] = camera.backgroundColor.g;
        renderOnce.backgroundColor[2] = camera.backgroundColor.b;
        renderOnce.backgroundColor[3] = camera.backgroundColor.a;
        
        RenderQueue::AddGlobalRenderCommand(renderOnce);

        for (const Entity* entity : entities)
        {
            Light* light = entity->GetComponent<Light>();
            Renderer* renderer = entity->GetComponent<Renderer>();
            MeshData* meshData = entity->GetComponent<MeshData>();

            if (light)
            {
                LightRenderCommand lightCommand;

                lightCommand.range = light->range;
                lightCommand.intensity = light->intensity;

                lightCommand.color[0] = light->color.r;
                lightCommand.color[1] = light->color.g;
                lightCommand.color[2] = light->color.b;

                lightCommand.attenuation[0] = 1.0f;     // ToDo! - Put them somewhere else.
                lightCommand.attenuation[1] = 0.09f;    // ToDo! - Put them somewhere else.
                lightCommand.attenuation[2] = 0.032f;   // ToDo! - Put them somewhere else.

                lightCommand.direction[0] = entity->transform.eulerAngles.x;
                lightCommand.direction[1] = entity->transform.eulerAngles.y;
                lightCommand.direction[2] = entity->transform.eulerAngles.z;

                lightCommand.position[0] = entity->transform.position.x;
                lightCommand.position[1] = entity->transform.position.y;
                lightCommand.position[2] = entity->transform.position.z;

                lightCommand.type = static_cast<int>(light->type);

                RenderQueue::AddLightRenderCommand(lightCommand);
            }

            if (renderer && meshData)
            {
                ObjectRenderCommand objectCommand;

                objectCommand.shader = &renderer->material.shader;
                objectCommand.texture = &renderer->material.texture;

                objectCommand.indices = meshData->mesh.GetIndices().data();
                objectCommand.indicesSize = meshData->mesh.GetIndices().size();

                // Convert Vector3/2 to floats: size x3 (Vector3), x2 (Vector2).
                objectCommand.vertices = reinterpret_cast<const float*>(meshData->mesh.GetVertices().data());
                objectCommand.verticesSize = meshData->mesh.GetVertices().size() * 3;

                objectCommand.texCoords = reinterpret_cast<const float*>(meshData->mesh.GetTexCoords().data());
                objectCommand.texCoordsSize = meshData->mesh.GetTexCoords().size() * 2;

                objectCommand.normals = reinterpret_cast<const float*>(meshData->mesh.GetNormals().data());
                objectCommand.normalsSize = meshData->mesh.GetNormals().size() * 3;

                Quaternion q = Quaternion::ToLHS(entity->transform.rotation);

                Matrix4 model = Matrix4::Identity();
                model = model.Scale(entity->transform.scale);
                model = model.Rotate(q);
                model = model.Translate(entity->transform.position);
                
                model.CopyToArray(objectCommand.modelMatrix);

                RenderQueue::AddObjectRenderCommand(objectCommand);
            }
        }
    }
}