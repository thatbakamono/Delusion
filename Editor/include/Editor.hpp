#pragma once

#include <filesystem>

#include <webgpu.h>

#include "delusion/Scene.hpp"
#include "delusion/graphics/Texture2D.hpp"

class Editor {
private:
    Entity *selectedEntity = nullptr;

    std::optional<std::filesystem::path> projectPath;

    std::filesystem::path assetsDirectory;
    std::filesystem::path currentDirectory;

    WGPUDevice device;
    WGPUQueue queue;

    std::shared_ptr<Texture2D> emptyTexture;
    std::shared_ptr<Texture2D> fileIconTexture;
    std::shared_ptr<Texture2D> directoryIconTexture;

    std::optional<Scene> m_scene;
public:
    Editor(WGPUDevice device, WGPUQueue queue, std::shared_ptr<Texture2D> emptyTexture,
           std::shared_ptr<Texture2D> fileIconTexture,
           std::shared_ptr<Texture2D> directoryIconTexture)
            : device(device), queue(queue), emptyTexture(std::move(emptyTexture)),
              fileIconTexture(std::move(fileIconTexture)), directoryIconTexture(std::move(directoryIconTexture)) {}

    void update(WGPUTextureView viewportTextureView);

    [[nodiscard]] std::optional<Scene>& scene() {
        return m_scene;
    }
private:
    bool entityHierarchy(Entity &entity);
};

