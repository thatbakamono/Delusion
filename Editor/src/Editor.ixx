module;

#include <optional>
#include <string>

#include <imgui.h>

#include <webgpu.h>

#include "delusion/Scene.hpp"

export module editor;

export class Editor {
public:
    void update(WGPUTextureView viewportTextureView, Scene& scene) {
        {
            ImGui::Begin("Hierarchy");

            if (ImGui::BeginPopupContextWindow("hierarchy_context_menu", 1)) {
                if (ImGui::MenuItem("Create entity")) {
                    scene.create();
                }

                ImGui::EndPopup();
            }

            auto& entities = scene.entities();

            std::optional<size_t> entityToDestroyIndex = {};

            for (size_t i = 0; i < entities.size(); i++) {
                auto& entity = entities[i];

                auto destroy = entityHierarchy(entity);

                if (destroy) {
                    entityToDestroyIndex = std::make_optional(i);
                }
            }

            if (entityToDestroyIndex.has_value()) {
                scene.remove(entities[entityToDestroyIndex.value()]);
            }

            ImGui::End();
        }

        {
            ImGui::Begin("Viewport");

            ImVec2 availableSpace = ImGui::GetContentRegionAvail();

            ImGui::Image(viewportTextureView, availableSpace);

            ImGui::End();
        }

        ImGui::Begin("Properties");

        ImGui::End();
    }
private:
    bool entityHierarchy(Entity& entity) {
        auto id = std::to_string(entity.id());

        auto isOpen = ImGui::TreeNode(id.c_str(), "Entity");
        auto destroy = false;

        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Create entity")) {
                entity.createChild();
            }

            if (ImGui::MenuItem("Delete entity")) {
                destroy = true;
            }

            ImGui::EndPopup();
        }

        if (isOpen) {
            auto children = entity.children();

            std::optional<size_t> entityToDestroyIndex = {};

            for (size_t i = 0; i < children.size(); i++) {
                auto& child = children[i];

                auto destroyChild = entityHierarchy(child);

                if (destroyChild) {
                    entityToDestroyIndex = std::make_optional(i);
                }
            }

            if (entityToDestroyIndex.has_value()) {
                entity.removeChild(children[entityToDestroyIndex.value()]);
            }

            ImGui::TreePop();
        }

        return destroy;
    }
};
