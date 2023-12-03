#pragma once

#include <filesystem>

#include <webgpu.h>

#include "delusion/Scene.hpp"
#include "delusion/graphics/Texture2D.hpp"

class Editor {
private:
    Entity* selectedEntity = nullptr;

    const std::filesystem::path assetsDirectory = "assets";
    std::filesystem::path currentDirectory = assetsDirectory;

    std::shared_ptr<Texture2D> fileIconTexture;
    std::shared_ptr<Texture2D> directoryIconTexture;
public:
    Editor(std::shared_ptr<Texture2D> fileIconTexture, std::shared_ptr<Texture2D> directoryIconTexture)
        : fileIconTexture(std::move(fileIconTexture)), directoryIconTexture(std::move(directoryIconTexture)) {}

    void update(WGPUTextureView viewportTextureView, Scene& scene);
private:
    bool entityHierarchy(Entity& entity);
};

