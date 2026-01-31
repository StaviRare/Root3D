#pragma once

#include "DeferredSceneLoad.h"
#include "SceneManager.h"
#include "Timer.h"

void DeferredSceneLoad::Tick()
{
    if (hasDelayElapsed == false)
    {
        elapsedTime += 1 * Timer::DeltaTime();

        if (elapsedTime > delayDuration)
        {
            hasDelayElapsed = true;
            SceneManager::LoadScene(sceneToLoad);
        }
    }
}