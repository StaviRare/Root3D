#pragma once

#include "MiscMove.h"
#include "Timer.h"
#include "Input.h"
#include "Entity.h"
#include "Debug.h"

void MiscMove::Tick()
{
    Transform& t = GetTransform();
    float deltaTime = Timer::DeltaTime();
    float timeSinceInit = Timer::TimeSinceInit();
    
    t.position += direction * deltaTime;
}