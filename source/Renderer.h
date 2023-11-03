#pragma once

#include "Material.h"
#include "Component.h"

class Renderer : public Component
{
public:
    Material material;
};
