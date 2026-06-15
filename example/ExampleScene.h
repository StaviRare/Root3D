#pragma once

#include "Scene.h"
#include "Entity.h"
#include "Shader.h"

class ExampleScene : public Scene
{
    public:
    void onLoad() override;
    void onUnload() override;

    private:
    void initCamera();
    void initShaders();
    void initLighting();
    void initStaticCube();
    void initDynamicCube();
    void initText();
    void unInit();

    private:
    Entity* m_staticCube = nullptr;
    Entity* m_dynamicCube = nullptr;
    Entity* m_dirLight = nullptr;
    Entity* m_pointLight = nullptr;
    Entity* m_playerController = nullptr;
    Entity* m_textEntity = nullptr;
    Shader* m_shaderLit = nullptr;
    Shader* m_shaderUnlit = nullptr;
};