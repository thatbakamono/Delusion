#include "Editor.hpp"

#include <fstream>
#include <optional>
#include <string>

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <nfd.hpp>

#include "delusion/Components.hpp"

void Editor::update(WGPUTextureView viewportTextureView, Scene &scene) {
    if (!projectPath.has_value()) {
        ImGui::Begin("Project");

        if (ImGui::Button("Create")) {
            // TODO: Support for non-ascii characters
            NFD::UniquePath path;

            if (NFD::PickFolder(path) == NFD_OKAY) {
                auto projectFilePath = std::filesystem::path(path.get());

                projectFilePath /= "project.delusion";

                std::ofstream(projectFilePath.string());

                projectPath = projectFilePath.parent_path();

                assetsDirectory = projectPath.value();
                assetsDirectory /= "assets";

                std::filesystem::create_directory(assetsDirectory);

                currentDirectory = assetsDirectory;
            }
        }

        if (ImGui::Button("Open")) {
            // TODO: Support for non-ascii characters
            NFD::UniquePath path;

            nfdu8filteritem_t filterItem = { "Project file", "delusion" };

            if (NFD::OpenDialog(path, &filterItem, 1) == NFD_OKAY) {
                auto projectFilePath = std::filesystem::path(path.get());

                projectPath = std::make_optional(projectFilePath.parent_path());
                assetsDirectory = projectPath.value();
                assetsDirectory /= "assets";
                currentDirectory = assetsDirectory;
            }
        }

        ImGui::End();
    } else {
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
                if (selectedEntity == &entities[entityToDestroyIndex.value()]) {
                    selectedEntity = nullptr;
                }

                scene.remove(entities[entityToDestroyIndex.value()]);
            }

            if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0)) {
                selectedEntity = nullptr;
            }

            ImGui::End();
        }

        {
            ImGui::Begin("Viewport");

            ImVec2 availableSpace = ImGui::GetContentRegionAvail();

            ImGui::Image(viewportTextureView, availableSpace);

            ImGui::End();
        }

        {
            ImGui::Begin("Asset browser");

            if (currentDirectory != assetsDirectory) {
                if (ImGui::Button("<-")) {
                    currentDirectory = currentDirectory.parent_path();
                }
            }

            auto availableSize = ImGui::GetContentRegionAvail();

            constexpr float padding = 8.0f;
            constexpr float thumbnailSize = 64.0f;

            auto totalSize = thumbnailSize + padding;
            auto columnCount = static_cast<int>(availableSize.x / totalSize);

            if (columnCount < 1)
                columnCount = 1;

            ImGui::Columns(columnCount, nullptr, false);

            for (const auto& entry : std::filesystem::directory_iterator(currentDirectory)) {
                const auto& path = entry.path();
                // TODO: Support for non-ascii characters
                std::string name = path.filename().string();

                auto iconTexture = entry.is_directory() ? directoryIconTexture : fileIconTexture;

                ImGui::ImageButton(iconTexture->view(), { thumbnailSize, thumbnailSize }, { 0.0f, 1.0f }, { 1.0f, 0.0f });

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    if (entry.is_directory()) {
                        currentDirectory /= path.filename();
                    }
                }

                ImGui::TextWrapped("%s", name.c_str());

                ImGui::NextColumn();
            }

            ImGui::Columns(1);

            ImGui::End();
        }

        {
            ImGui::Begin("Properties");

            if (selectedEntity != nullptr) {
                auto hasTransform = selectedEntity->hasComponent<Transform>();
                auto hasSprite = selectedEntity->hasComponent<Sprite>();

                if (hasTransform) {
                    auto& transform = selectedEntity->getComponent<Transform>();

                    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                        if (ImGui::BeginPopupContextItem(nullptr)) {
                            if (ImGui::MenuItem("Remove component")) {
                                selectedEntity->removeComponent<Transform>();
                            }

                            ImGui::EndPopup();
                        }

                        ImGui::DragFloat2("Position", glm::value_ptr(transform.position), 0.1f, 0.0f, 0.0f, "%.5f");
                        ImGui::DragFloat("Rotation", &transform.rotation, 0.1f, 0.0f, 0.0f, "%.5f");
                        ImGui::DragFloat2("Scale", glm::value_ptr(transform.scale), 0.1f, 0.0f, 0.0f, "%.5f");
                    }
                }

                if (hasSprite) {
                    auto& sprite = selectedEntity->getComponent<Sprite>();

                    if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
                        if (ImGui::BeginPopupContextItem(nullptr)) {
                            if (ImGui::MenuItem("Remove component")) {
                                selectedEntity->removeComponent<Sprite>();
                            }

                            ImGui::EndPopup();
                        }

                        if (sprite.texture != nullptr) {
                            ImGui::Image(sprite.texture->view(), ImVec2(128.0f, 128.0f), ImVec2(0, 1), ImVec2(1, 0));
                        }
                    }
                }

                if (ImGui::Button("Add component")) {
                    ImGui::OpenPopup("add_component_popup");
                }

                if (ImGui::BeginPopup("add_component_popup")) {
                    if (!hasTransform) {
                        if (ImGui::Button("Transform")) {
                            selectedEntity->addComponent<Transform>();

                            ImGui::CloseCurrentPopup();
                        }
                    }

                    if (!hasSprite) {
                        if (ImGui::Button("Sprite")) {
                            selectedEntity->addComponent<Sprite>();

                            ImGui::CloseCurrentPopup();
                        }
                    }

                    ImGui::EndPopup();
                }
            }

            ImGui::End();
        }
    }
}

bool Editor::entityHierarchy(Entity &entity) {
    auto id = std::to_string(entity.id());

    auto isOpen = ImGui::TreeNode(id.c_str(), "Entity");
    auto destroy = false;

    if (ImGui::IsItemClicked()) {
        selectedEntity = &entity;
    }

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
            if (selectedEntity == &children[entityToDestroyIndex.value()]) {
                selectedEntity = nullptr;
            }

            entity.removeChild(children[entityToDestroyIndex.value()]);
        }

        ImGui::TreePop();
    }

    return destroy;
}
