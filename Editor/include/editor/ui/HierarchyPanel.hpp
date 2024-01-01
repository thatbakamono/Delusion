#pragma once

#include <optional>

#include <imgui.h>

#include <delusion/Engine.hpp>
#include <delusion/Entity.hpp>

class HierarchyPanel {
    private:
        Engine *m_engine {};

        Entity *m_selectedEntity {};
    public:
        explicit HierarchyPanel(Engine *engine) : m_engine(engine) {}

        void onUpdate();

        Entity *selectedEntity() {
            return m_selectedEntity;
        }

        void setSelectedEntity(Entity *entity) {
            m_selectedEntity = entity;
        }
    private:
        bool entityHierarchy(Entity &entity);
};
