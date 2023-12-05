#include "Editor.hpp"

#include <fstream>
#include <optional>
#include <string>

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <nfd.hpp>

#include "delusion/Components.hpp"
#include "delusion/SceneSerde.hpp"
#include "delusion/formats/ImageDecoder.hpp"
#include "delusion/io/FileUtilities.hpp"

void Editor::update(WGPUTextureView viewportTextureView) {
    if (!m_project.has_value()) {
        onProjectPanel();
    } else {
        auto& project = m_project.value();

        onMenuBar(project);
        onHierarchyPanel();
        onViewportPanel(viewportTextureView);
        onAssetBrowserPanel(project);
        onPropertiesPanel();
    }
}

void Editor::onProjectPanel() {
    ImGui::Begin("Project");

    if (ImGui::Button("Create")) {
        // TODO: Support for non-ascii characters
        NFD::UniquePath path;

        if (NFD::PickFolder(path) == NFD_OKAY) {
            auto projectFilePath = std::filesystem::path(path.get());

            projectFilePath /= "project.delusion";

            std::ofstream(projectFilePath.string());

            m_project = std::make_optional(Project(projectFilePath.parent_path()));

            std::filesystem::create_directory(m_project->assetsDirectoryPath());

            m_currentDirectory = m_project->assetsDirectoryPath();
        }
    }

    if (ImGui::Button("Open")) {
        // TODO: Support for non-ascii characters
        NFD::UniquePath path;

        nfdu8filteritem_t filterItem = { "Project file", "delusion" };

        if (NFD::OpenDialog(path, &filterItem, 1) == NFD_OKAY) {
            auto projectFilePath = std::filesystem::path(path.get());
            auto projectPath = projectFilePath.parent_path();

            m_project = std::make_optional(Project(projectFilePath.parent_path()));
            m_currentDirectory = m_project->assetsDirectoryPath();
        }
    }

    ImGui::End();
}

void Editor::onMenuBar(Project& project) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Scene")) {
            if (ImGui::MenuItem("Save", nullptr, false, m_scene.has_value())) {
                // TODO: Support for non-ascii characters
                NFD::UniquePath path;

                auto assetsDirectoryString = project.assetsDirectoryPath().string();

                nfdu8filteritem_t filterItem = { "Scene file", "scene" };

                if (NFD::SaveDialog(path, &filterItem, 1, assetsDirectoryString.c_str()) == NFD_OKAY) {
                    std::ofstream stream(path.get());

                    stream << SceneSerde::serialize(m_scene.value());
                }
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Editor::onHierarchyPanel() {
    ImGui::Begin("Hierarchy");

    if (m_scene.has_value()) {
        auto& currentScene = m_scene.value();

        if (ImGui::BeginPopupContextWindow("hierarchy_context_menu", 1)) {
            if (ImGui::MenuItem("Create entity")) {
                currentScene.create();
            }

            ImGui::EndPopup();
        }

        auto& entities = currentScene.entities();

        std::optional<size_t> entityToDestroyIndex = {};

        for (size_t i = 0; i < entities.size(); i++) {
            auto& entity = entities[i];

            auto destroy = entityHierarchy(entity);

            if (destroy) {
                entityToDestroyIndex = std::make_optional(i);
            }
        }

        if (entityToDestroyIndex.has_value()) {
            if (m_selectedEntity == &entities[entityToDestroyIndex.value()]) {
                m_selectedEntity = nullptr;
            }

            currentScene.remove(entities[entityToDestroyIndex.value()]);
        }

        if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0)) {
            m_selectedEntity = nullptr;
        }
    }

    ImGui::End();
}

void Editor::onViewportPanel(WGPUTextureView viewportTextureView) {
    ImGui::Begin("Viewport");

    ImVec2 availableSpace = ImGui::GetContentRegionAvail();

    ImGui::Image(viewportTextureView, availableSpace);

    if (ImGui::BeginDragDropTarget()) {
        auto payload = ImGui::AcceptDragDropPayload("scene");

        if (payload != nullptr) {
            auto source = readAsString(static_cast<char*>(payload->Data));

            if (source.has_value()) {
                m_selectedEntity = nullptr;
                m_scene = std::make_optional(SceneSerde::deserialize(source.value()));
            }
        }

        ImGui::EndDragDropTarget();
    }

    ImGui::End();
}

void Editor::onAssetBrowserPanel(Project& project) {
    ImGui::Begin("Asset browser");

    if (m_currentDirectory != project.assetsDirectoryPath()) {
        if (ImGui::Button("<-")) {
            m_currentDirectory = m_currentDirectory.parent_path();
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

    for (const auto& entry : std::filesystem::directory_iterator(m_currentDirectory)) {
        const auto& path = entry.path();
        // TODO: Support for non-ascii characters
        std::string name = path.filename().string();

        auto iconTexture = entry.is_directory() ? m_directoryIconTexture : m_fileIconTexture;

        auto extension = path.filename().extension();
        auto extensionText = extension.string();

        ImGui::PushID(name.c_str());

        ImGui::ImageButton(iconTexture->view(), { thumbnailSize, thumbnailSize }, { 0.0f, 1.0f }, { 1.0f, 0.0f });

        if (!entry.is_directory()) {
            // TODO: Ask some kind of FormatRegistry whether it's a supported image format
            if (extensionText == ".png") {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                    auto pathText = path.string();

                    ImGui::SetDragDropPayload("image", pathText.c_str(), pathText.size() + 1, ImGuiCond_Once);

                    ImGui::EndDragDropSource();
                }
            }

            if (extensionText == ".scene") {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                    auto pathText = path.string();

                    ImGui::SetDragDropPayload("scene", pathText.c_str(), pathText.size() + 1, ImGuiCond_Once);

                    ImGui::EndDragDropSource();
                }
            }
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            if (entry.is_directory()) {
                m_currentDirectory /= path.filename();
            }
        }

        ImGui::TextWrapped("%s", name.c_str());

        ImGui::NextColumn();

        ImGui::PopID();
    }

    ImGui::Columns(1);

    ImGui::End();
}

void Editor::onPropertiesPanel() {
    ImGui::Begin("Properties");

    if (m_selectedEntity != nullptr) {
        auto hasTransform = m_selectedEntity->hasComponent<Transform>();
        auto hasSprite = m_selectedEntity->hasComponent<Sprite>();

        if (hasTransform) {
            auto& transform = m_selectedEntity->getComponent<Transform>();

            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::BeginPopupContextItem(nullptr)) {
                    if (ImGui::MenuItem("Remove component")) {
                        m_selectedEntity->removeComponent<Transform>();
                    }

                    ImGui::EndPopup();
                }

                ImGui::DragFloat2("Position", glm::value_ptr(transform.position), 0.1f, 0.0f, 0.0f, "%.5f");
                ImGui::DragFloat("Rotation", &transform.rotation, 0.1f, 0.0f, 0.0f, "%.5f");
                ImGui::DragFloat2("Scale", glm::value_ptr(transform.scale), 0.1f, 0.0f, 0.0f, "%.5f");
            }
        }

        if (hasSprite) {
            auto& sprite = m_selectedEntity->getComponent<Sprite>();

            if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::BeginPopupContextItem(nullptr)) {
                    if (ImGui::MenuItem("Remove component")) {
                        m_selectedEntity->removeComponent<Sprite>();
                    }

                    ImGui::EndPopup();
                }

                if (sprite.texture != nullptr) {
                    ImGui::Image(sprite.texture->view(), ImVec2(128.0f, 128.0f), ImVec2(0, 1), ImVec2(1, 0));
                } else {
                    ImGui::Image(m_emptyTexture->view(), ImVec2(128.0f, 128.0f), ImVec2(0, 1), ImVec2(1, 0));
                }

                if (ImGui::BeginDragDropTarget()) {
                    auto payload = ImGui::AcceptDragDropPayload("image");

                    if (payload != nullptr) {
                        auto image = ImageDecoder::decode(static_cast<char*>(payload->Data));
                        std::shared_ptr<Texture2D> texture = Texture2D::create(m_device, m_queue, image);

                        sprite.texture = texture;
                    }

                    ImGui::EndDragDropTarget();
                }
            }
        }

        if (ImGui::Button("Add component")) {
            ImGui::OpenPopup("add_component_popup");
        }

        if (ImGui::BeginPopup("add_component_popup")) {
            if (!hasTransform) {
                if (ImGui::Button("Transform")) {
                    m_selectedEntity->addComponent<Transform>();

                    ImGui::CloseCurrentPopup();
                }
            }

            if (!hasSprite) {
                if (ImGui::Button("Sprite")) {
                    m_selectedEntity->addComponent<Sprite>();

                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }
    }

    ImGui::End();
}

bool Editor::entityHierarchy(Entity &entity) {
    auto id = std::to_string(entity.id());

    auto isOpen = ImGui::TreeNode(id.c_str(), "Entity");
    auto destroy = false;

    if (ImGui::IsItemClicked()) {
        m_selectedEntity = &entity;
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
        auto& children = entity.children();

        std::optional<size_t> entityToDestroyIndex = {};

        for (size_t i = 0; i < children.size(); i++) {
            auto& child = children[i];

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
