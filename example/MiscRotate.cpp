#pragma once

#include "MiscRotate.h"

#include "Timer.h"
#include "Input.h"
#include "Entity.h"
#include "Debug.h"

static Vector3 direction = Vector3(-5, -5, 5);

void MiscRotate::Tick()
{
    Transform& t = GetTransform();
    float deltaTime = Timer::DeltaTime();
    float timeSinceInit = Timer::TimeSinceInit();
    
    t.eulerAngles += direction * deltaTime;
    t.rotation = Quaternion::FromEuler(t.eulerAngles);
}