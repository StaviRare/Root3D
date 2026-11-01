#include "SplashScene.h"
#include "MeshFilter.h"
#include "Renderer.h"
#include "Camera.h"
#include "DeferredSceneLoad.h"

void SplashScene::onLoad()
{
    // Camera
    m_camera = new Entity();
    m_camera->transform.position = Vector3(0, 0, 1.5f);
    Camera* camera = m_camera->AddComponent<Camera>();
    camera->backgroundColor = Color(0, 0, 0, 1);

    // Logo
    m_shader = new Shader("shaders/unlit");
    m_logo = new Entity();
    m_logo->transform.scale = Vector3(1, 1, 1);
    m_logo->transform.position = Vector3(0, 0, 0);
    m_logo->transform.rotation = Quaternion::FromEuler(Vector3(0, 0, 0));
    m_mesh = new Mesh();
    m_mesh->CreatePrimitive(PrimitiveType::Quad);
    MeshFilter* meshFilter = m_logo->AddComponent<MeshFilter>();
    Renderer* renderer = m_logo->AddComponent<Renderer>();
    Material material(*m_shader);
    m_texture = new Texture("textures/logo.png");
    material.texture = m_texture;
    meshFilter->mesh = m_mesh;
    renderer->material = material;

    // Scene Loader
    m_sceneLoader = new Entity();
    DeferredSceneLoad* sceneLoader = m_sceneLoader->AddComponent<DeferredSceneLoad>();
    sceneLoader->delayDuration = 3;
    sceneLoader->sceneToLoad = 1;
}

void SplashScene::onUnload()
{
    delete m_mesh;
    m_mesh = nullptr;

    delete m_shader;
    m_shader = nullptr;

    delete m_texture;
    m_texture = nullptr;

    delete m_camera;
    m_camera = nullptr;

    delete m_logo;
    m_logo = nullptr;

    delete m_sceneLoader;
    m_sceneLoader = nullptr;
}