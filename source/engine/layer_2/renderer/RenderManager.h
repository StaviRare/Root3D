#pragma once

#include <set>
#include <unordered_map>

#include "Entity.h"
#include "Camera.h"
#include "Graphics.h"

class RenderManager
{
    friend class Engine;

    private:
    static void Initialize();
    static void PreRender();
    static void Render();
    static void PostRender();
    static void UnInitialize();
    static void CollectEntities();
    static void UpdateFrameUniforms();
    static void CullEntities();
    static void BeginRenderPass();
    static void DrawEntities();
    static void EndRenderPass();
    static void PresentFrame();

    private:
    static Camera* _camera;
    static std::set<Entity*> _entities;
    static FrameUniform _frameUniforms;
};
