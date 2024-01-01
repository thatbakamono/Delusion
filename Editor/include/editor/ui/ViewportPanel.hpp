#pragma once

#include "editor/Project.hpp"
#include "editor/ui/HierarchyPanel.hpp"

#include <delusion/Engine.hpp>
#include <delusion/graphics/OrthographicCamera.hpp>
#include <delusion/graphics/Texture2D.hpp>
#include <delusion/Scene.hpp>
#include <delusion/SceneSerde.hpp>
#include <delusion/scripting/ScriptEngine.hpp>

class Editor;

class ViewportPanel {
    private:
        Editor &m_editor;

        HierarchyPanel &m_hierarchyPanel;

        Engine *m_engine;

        std::shared_ptr<ScriptEngine> m_scriptEngine;

        std::shared_ptr<Texture2D> m_playIconTexture;
        std::shared_ptr<Texture2D> m_stopIconTexture;

        SceneSerde m_sceneSerde;

        OrthographicCamera m_camera = OrthographicCamera(glm::vec3(0.0f, 0.0f, -1.0f));
    public:
        ViewportPanel(
            Editor &editor, HierarchyPanel &hierarchyPanel, Engine *engine, std::shared_ptr<ScriptEngine> scriptEngine,
            std::shared_ptr<Texture2D> playIconTexture, std::shared_ptr<Texture2D> stopIconTexture
        );

        void onUpdate(Project &project, std::shared_ptr<Texture2D> &viewportTexture, float deltaTime);

        [[nodiscard]] OrthographicCamera &camera() {
            return m_camera;
        }
};
