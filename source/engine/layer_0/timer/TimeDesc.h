#pragma once

#include "Types.h"

// ToDo - Implement. Maybe timer should do less.

struct TimeDesc
{
	float timeScale = 1.00f;
	float maxDeltaTime = 0.10f;
	float fixedTimeStep = 0.02f;
};