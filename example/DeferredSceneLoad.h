#pragma once

#include "Component.h"

class DeferredSceneLoad : public Component
{
    public:
    void Tick() override;

    public:
    float delayDuration = 1;
    int sceneToLoad = 0;

    private:
    float elapsedTime = 0;
    bool hasDelayElapsed = false;
};