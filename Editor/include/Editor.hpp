#pragma once

#include <webgpu.h>

#include "delusion/Scene.hpp"

class Editor {
private:
    Entity* selectedEntity = nullptr;
public:
    void update(WGPUTextureView viewportTextureView, Scene& scene);
private:
    bool entityHierarchy(Entity& entity);
};

