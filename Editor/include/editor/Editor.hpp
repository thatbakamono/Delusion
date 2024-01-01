#pragma once

#include <filesystem>

#include <FileWatch.hpp>

#include "editor/Project.hpp"
#include "editor/ui/AssetBrowserPanel.hpp"
#include "editor/ui/HierarchyPanel.hpp"
#include "editor/ui/PropertiesPanel.hpp"
#include "editor/ui/ViewportPanel.hpp"

#include <delusion/AssetManager.hpp>
#include <delusion/Engine.hpp>
#include <delusion/graphics/OrthographicCamera.hpp>
#include <delusion/graphics/Texture2D.hpp>
#include <delusion/Scene.hpp>
#include <delusion/SceneSerde.hpp>
#include <delusion/scripting/ScriptEngine.hpp>

class Editor {
    private:
        Engine *m_engine;

        std::shared_ptr<AssetManager> m_assetManager;

        std::optional<Project> m_project;

        std::shared_ptr<Scene> m_scene = std::make_shared<Scene>();

        std::unique_ptr<filewatch::FileWatch<std::string>> m_fileWatch;

        std::optional<std::filesystem::path> m_fileBeingRenamed;

        SceneSerde m_sceneSerde;

        HierarchyPanel m_hierarchyPanel;
        ViewportPanel m_viewportPanel;
        AssetBrowserPanel m_assetBrowserPanel;
        PropertiesPanel m_propertiesPanel;

        bool m_isPlaying = false;
    public:
        Editor(
            Engine *engine, std::shared_ptr<ScriptEngine> scriptEngine, std::shared_ptr<Texture2D> emptyTexture,
            std::shared_ptr<Texture2D> fileIconTexture, std::shared_ptr<Texture2D> directoryIconTexture,
            std::shared_ptr<Texture2D> playIconTexture, std::shared_ptr<Texture2D> stopIconTexture
        );

        void onEditorUpdate(std::shared_ptr<Texture2D> &viewportTexture, float deltaTime);
        void onRuntimeUpdate(float deltaTime);

        [[nodiscard]] std::shared_ptr<Scene> &scene() {
            return m_scene;
        }

        void setScene(std::shared_ptr<Scene> scene) {
            m_scene = std::move(scene);
        }

        [[nodiscard]] OrthographicCamera &camera() {
            return m_viewportPanel.camera();
        }

        [[nodiscard]] bool isPlaying() const {
            return m_isPlaying;
        }

        void setPlaying(bool playing) {
            m_isPlaying = playing;
        }
    private:
        void onProjectPanel();
        void onMenuBar(Project &project);

        void onFileSystemChange(const std::string &path, const filewatch::Event event);
};
