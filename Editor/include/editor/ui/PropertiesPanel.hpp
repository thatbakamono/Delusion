#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <utility>

#include "editor/ui/HierarchyPanel.hpp"

#include <delusion/Components.hpp>
#include <delusion/scripting/ScriptEngine.hpp>

class Editor;

class PropertiesPanel {
    private:
        Editor &editor;

        HierarchyPanel &m_hierarchyPanel;

        std::shared_ptr<Texture2D> m_emptyTexture;

        std::shared_ptr<AssetManager> m_assetManager;
    public:
        PropertiesPanel(
            Editor &editor, HierarchyPanel &hierarchyPanel, std::shared_ptr<AssetManager> assetManager,
            std::shared_ptr<Texture2D> emptyTexture
        )
            : m_hierarchyPanel(hierarchyPanel), m_emptyTexture(std::move(emptyTexture)),
              m_assetManager(std::move(assetManager)), editor(editor) {}

        void onUpdate();
};
