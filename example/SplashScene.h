#pragma once

#include "Scene.h"
#include "Entity.h"
#include "Shader.h"

class SplashScene : public Scene
{
    public:
    void onLoad() override;
    void onUnload() override;

    private:
    void initShaders();

    private:
    Shader* m_shader = nullptr;
    Entity* m_camera = nullptr;
    Entity* m_logo = nullptr;
    Entity* m_sceneLoader = nullptr;
};