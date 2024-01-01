#include "editor/ui/HierarchyPanel.hpp"

void HierarchyPanel::onUpdate() {
    ImGui::Begin("Hierarchy");

    if (m_engine->currentScene() != nullptr) {
        auto *scene = m_engine->currentScene();

        if (ImGui::BeginPopupContextWindow("hierarchy_context_menu", 1)) {
            if (ImGui::MenuItem("Create entity")) {
                auto &entities = scene->entities();

                std::optional<size_t> selectedEntityIndex = std::nullopt;

                if (m_selectedEntity != nullptr) {
                    for (size_t i = 0; i < entities.size(); i++) {
                        if (*m_selectedEntity == entities[i]) {
                            selectedEntityIndex = std::make_optional(i);
                        }
                    }
                }

                scene->create();

                if (selectedEntityIndex.has_value()) {
                    m_selectedEntity = &entities[selectedEntityIndex.value()];
                }
            }

            ImGui::EndPopup();
        }

        auto &entities = scene->entities();

        std::optional<size_t> entityToDestroyIndex = {};

        for (size_t i = 0; i < entities.size(); i++) {
            auto &entity = entities[i];

            auto destroy = entityHierarchy(entity);

            if (destroy) {
                entityToDestroyIndex = std::make_optional(i);
            }
        }

        if (entityToDestroyIndex.has_value()) {
            if (m_selectedEntity == &entities[entityToDestroyIndex.value()]) {
                m_selectedEntity = nullptr;
            }

            scene->remove(entities[entityToDestroyIndex.value()]);
        }

        if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0)) {
            m_selectedEntity = nullptr;
        }
    }

    ImGui::End();
}

bool HierarchyPanel::entityHierarchy(Entity &entity) {
    auto id = std::to_string(entity.id().value());

    auto isOpen = ImGui::TreeNode(id.c_str(), "Entity");
    auto destroy = false;

    if (ImGui::IsItemClicked()) {
        m_selectedEntity = &entity;
    }

    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Create entity")) {
            auto &children = entity.children();

            std::optional<size_t> selectedEntityIndex = std::nullopt;

            if (m_selectedEntity != nullptr) {
                for (size_t i = 0; i < children.size(); i++) {
                    if (*m_selectedEntity == children[i]) {
                        selectedEntityIndex = std::make_optional(i);
                    }
                }
            }

            entity.createChild();

            if (selectedEntityIndex.has_value()) {
                m_selectedEntity = &children[selectedEntityIndex.value()];
            }
        }

        if (ImGui::MenuItem("Delete entity")) {
            destroy = true;
        }

        ImGui::EndPopup();
    }

    if (isOpen) {
        auto &children = entity.children();

        std::optional<size_t> entityToDestroyIndex = {};

        for (size_t i = 0; i < children.size(); i++) {
            auto &child = children[i];

            auto destroyChild = entityHierarchy(child);

            if (destroyChild) {
                entityToDestroyIndex = std::make_optional(i);
            }
        }

        if (entityToDestroyIndex.has_value()) {
            if (m_selectedEntity == &children[entityToDestroyIndex.value()]) {
                m_selectedEntity = nullptr;
            }

            entity.removeChild(children[entityToDestroyIndex.value()]);
        }

        ImGui::TreePop();
    }

    return destroy;
}
