#pragma once

#include "Vector3.h"
#include "Collider.h"

class BoxCollider : public Collider
{
	public:
	Vector3 size = Vector3(1, 1, 1);
	Vector3 center = Vector3(0, 0, 0);
};
