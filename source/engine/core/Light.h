#pragma once

#include "Color.h"
#include "Component.h"

enum class LightType
{
	Directional,
	Point
};

class Light : public Component
{
	public:
	float intensity = 1.0f;
	Color color = Color(1, 1, 1);
	LightType type = LightType::Directional;

	// Spot
	float range = 1.0f;
};