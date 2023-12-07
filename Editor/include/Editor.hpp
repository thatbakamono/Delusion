#pragma once

#include <filesystem>

#include <webgpu.h>
#include <FileWatch.hpp>

#include "Project.hpp"
#include "delusion/AssetManager.hpp"
#include "delusion/Scene.hpp"
#include "delusion/SceneSerde.hpp"
#include "delusion/graphics/Texture2D.hpp"

class Editor {
private:
    WGPUDevice m_device;
    WGPUQueue m_queue;

    std::shared_ptr<Texture2D> m_emptyTexture;
    std::shared_ptr<Texture2D> m_fileIconTexture;
    std::shared_ptr<Texture2D> m_directoryIconTexture;

    std::shared_ptr<AssetManager> m_assetManager;

    SceneSerde m_sceneSerde;

    std::optional<Project> m_project;

    Entity *m_selectedEntity = nullptr;

    std::filesystem::path m_currentDirectory;

    std::optional<Scene> m_scene;

    std::unique_ptr<filewatch::FileWatch<std::string>> m_fileWatch;

    std::optional<std::filesystem::path> m_fileBeingRenamed;
public:
    Editor(WGPUDevice device, WGPUQueue queue, std::shared_ptr<Texture2D> emptyTexture,
           std::shared_ptr<Texture2D> fileIconTexture,
           std::shared_ptr<Texture2D> directoryIconTexture)
            : m_device(device), m_queue(queue), m_emptyTexture(std::move(emptyTexture)),
              m_fileIconTexture(std::move(fileIconTexture)), m_directoryIconTexture(std::move(directoryIconTexture)),
              m_assetManager(std::make_shared<AssetManager>(device, queue)), m_sceneSerde(m_assetManager) {}

    void update(WGPUTextureView viewportTextureView);

    [[nodiscard]] std::optional<Scene>& scene() {
        return m_scene;
    }
private:
    void onProjectPanel();
    void onMenuBar(Project& project);
    void onHierarchyPanel();
    void onViewportPanel(WGPUTextureView viewportTextureView);
    void onAssetBrowserPanel(Project& project);
    void onPropertiesPanel();

    bool entityHierarchy(Entity &entity);

    void onFileSystemChange(const std::string& path, const filewatch::Event event);
};

