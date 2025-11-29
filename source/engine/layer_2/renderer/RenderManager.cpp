#include "RenderManager.h"
#include "MeshFilter.h"
#include "Mesh.h"
#include "GraphicsTypes.h"
#include "Renderer.h"
#include "Debug.h"
#include "Matrix4.h"
#include "Light.h"
#include "SceneManager.h"

Camera* RenderManager::_camera = nullptr;
std::set<Entity*> RenderManager::_entities;
FrameUniform RenderManager::_frameUniforms = {};

void RenderManager::Initialize()
{
	// Empty at the moment.
}

void RenderManager::UnInitialize()
{
	// Empty at the moment.
}

void RenderManager::PreRender()
{
	_camera = Camera::GetInstance();

	if (_camera)
	{
		CollectEntities();
		CullEntities();
		UpdateFrameUniforms();
	}
}

void RenderManager::Render()
{
	if (_camera)
	{
		BeginRenderPass();
		DrawEntities();
	}
}

void RenderManager::PostRender()
{
	if (_camera)
	{
		EndRenderPass();
		PresentFrame();
	}
}

void RenderManager::CollectEntities()
{
	Scene* currentScene = SceneManager::GetCurrentScene();

	if (currentScene)
	{
		_entities = currentScene->GetEntities();
	}
}

void RenderManager::CullEntities()
{
	Vector3 cameraPos = _camera->GetTransform().position;
	Vector3 cameraForward = _camera->GetTransform().getForward();

	auto it = _entities.begin();

	while (it != _entities.end())
	{
		Entity* entity = *it;
		Renderer* renderer = entity->GetComponent<Renderer>();
		MeshFilter* meshFilter = entity->GetComponent<MeshFilter>();

		// Remove entities with no renderer/meshFilter
		if (!renderer || !meshFilter)
		{
			it = _entities.erase(it);
		}
		else
		{
			Vector3 toEntity = entity->transform.position - cameraPos;

			// Remove entities behind the camera
			if (toEntity.dot(cameraForward) <= 0.0f)
			{
				it = _entities.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

void RenderManager::UpdateFrameUniforms()
{
	Vector3 cameraUp(0.0f, 1.0f, 0.0f);
	Vector3 cameraTarget = _camera->GetTransform().position + _camera->GetTransform().getForward();
	Matrix4 view = Matrix4::LookAt(_camera->GetTransform().position, cameraTarget, cameraUp);
	Matrix4 projection = Matrix4::Perspective(_camera->fov, _camera->GetAspect(), _camera->nearClipPlane, _camera->farClipPlane);
	
	view.CopyToArray(_frameUniforms.viewMatrix);
	projection.CopyToArray(_frameUniforms.projectionMatrix);
	
	_frameUniforms.backgroundColor[0] = _camera->backgroundColor.r;
	_frameUniforms.backgroundColor[1] = _camera->backgroundColor.g;
	_frameUniforms.backgroundColor[2] = _camera->backgroundColor.b;
	_frameUniforms.backgroundColor[3] = _camera->backgroundColor.a;

	// Ligths
	unsigned int index = 0;
	const int maxLigths = 20;

	Scene* currentScene = SceneManager::GetCurrentScene();
	auto _entities2 = currentScene->GetEntities();
	

	for (Entity* entity : _entities2)
	{
		Light* light = entity->GetComponent<Light>();

		if (light && index < maxLigths)
		{
			LightUniform lightUniform;

			lightUniform.range = light->range;
			lightUniform.intensity = light->intensity;

			lightUniform.color[0] = light->color.r;
			lightUniform.color[1] = light->color.g;
			lightUniform.color[2] = light->color.b;

			lightUniform.attenuation[0] = 1.0f;     // ToDo! - Put them somewhere else.
			lightUniform.attenuation[1] = 0.09f;    // ToDo! - Put them somewhere else.
			lightUniform.attenuation[2] = 0.032f;   // ToDo! - Put them somewhere else.

			lightUniform.direction[0] = entity->transform.eulerAngles.x;
			lightUniform.direction[1] = entity->transform.eulerAngles.y;
			lightUniform.direction[2] = entity->transform.eulerAngles.z;

			lightUniform.position[0] = entity->transform.position.x;
			lightUniform.position[1] = entity->transform.position.y;
			lightUniform.position[2] = entity->transform.position.z;

			lightUniform.type = static_cast<int>( light->type );

			_frameUniforms.lights[index] = lightUniform;
			index += 1;
		}
	}
}

void RenderManager::BeginRenderPass()
{
	Graphics::BeginFrame(_frameUniforms);
}

void RenderManager::DrawEntities()
{
	for (Entity* entity : _entities)
	{
		Renderer* renderer = entity->GetComponent<Renderer>();
		MeshFilter* meshFilter = entity->GetComponent<MeshFilter>();

		if (renderer && meshFilter)
		{
			// Mesh
			// Convert Vector3/2 to floats: size x3 (Vector3), x2 (Vector2).
			MeshUpload meshData;
			meshData.indices = meshFilter->mesh.GetIndices().data();
			meshData.indicesSize = meshFilter->mesh.GetIndices().size();
			meshData.vertices = reinterpret_cast<const float*>(meshFilter->mesh.GetVertices().data());
			meshData.verticesSize = meshFilter->mesh.GetVertices().size() * 3;
			meshData.texCoords = reinterpret_cast<const float*>(meshFilter->mesh.GetTexCoords().data());
			meshData.texCoordsSize = meshFilter->mesh.GetTexCoords().size() * 2;
			meshData.normals = reinterpret_cast<const float*>(meshFilter->mesh.GetNormals().data());
			meshData.normalsSize = meshFilter->mesh.GetNormals().size() * 3;

			// Command
			ObjectUniform objectCommand;
			objectCommand.mesh = meshData;

			// Texture
			uniqueID textureId = renderer->material.texture->getID();
			unsigned int textureHandle = TextureManager::GetHandle(textureId);
			objectCommand.textureHandle = textureHandle;

			// Shader
			uniqueID shaderId = renderer->material.GetShaderID();
			unsigned int shaderHandle = ShaderManager::GetHandle(shaderId);
			objectCommand.shaderHandle = shaderHandle;

			// Model Matrix
			Quaternion q = Quaternion::ToLHS(entity->transform.rotation);
			Matrix4 model = Matrix4::Identity();
			model = model.Scale(entity->transform.scale);
			model = model.Rotate(q);
			model = model.Translate(entity->transform.position);
			model.CopyToArray(objectCommand.modelMatrix);

			Graphics::DrawObject(objectCommand);
		}
	}
}

void RenderManager::EndRenderPass()
{
	_entities.clear();
}

void RenderManager::PresentFrame()
{
	Graphics::EndFrame();
}