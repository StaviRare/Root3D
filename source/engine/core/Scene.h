#pragma once

class Scene {
public:
    virtual ~Scene() {}

    // Initialization function for the scene
    virtual void init() = 0;

    // Ticking function, called regularly to update the scene
    virtual void tick() = 0;
};
