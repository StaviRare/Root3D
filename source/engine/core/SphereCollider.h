#pragma once

#include "Vector3.h"
#include "Collider.h"

class SphereCollider : public Collider
{
	public:
	float radius = 1.0f;
	Vector3 center = Vector3(0, 0, 0);
};
