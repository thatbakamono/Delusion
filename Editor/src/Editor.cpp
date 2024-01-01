#include "Editor.hpp"

#include <fstream>
#include <optional>
#include <string>

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <nfd.hpp>

#include "delusion/Components.hpp"
#include "delusion/io/FileUtilities.hpp"
#include "delusion/scripting/Internals.hpp"
#include "delusion/Utilities.hpp"

void Editor::onEditorUpdate(std::shared_ptr<Texture2D> &viewportTexture, float deltaTime) {
    if (!m_project.has_value()) {
        onProjectPanel();
    } else {
        auto &project = m_project.value();

        onMenuBar(project);
        onHierarchyPanel();
        onViewportPanel(viewportTexture, deltaTime);
        onAssetBrowserPanel(project);
        onPropertiesPanel();
    }
}

void Editor::onRuntimeUpdate(float deltaTime) {
    if (m_project.has_value()) {
        if (isPlaying) {
            m_engine->currentScene()->forEachEntity([&deltaTime](Entity &entity) {
                if (entity.hasComponent<ScriptComponent>()) {
                    auto &script = entity.getComponent<ScriptComponent>();

                    auto scriptClass = CSharpClass(static_cast<MonoClass *>(script.class_));
                    auto instance = CSharpObject(static_cast<MonoObject *>(script.instance));

                    auto onCreateMethod = scriptClass.getMethod("OnUpdate", 1);

                    float *deltaTimePtr = &deltaTime;

                    mono_runtime_invoke(
                        onCreateMethod, instance.getMonoObject(), reinterpret_cast<void **>(&deltaTimePtr), nullptr
                    );
                }
            });

            m_engine->currentScene()->onUpdate(deltaTime);
        }
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

            m_fileWatch = std::make_unique<filewatch::FileWatch<std::string>>(
                m_project->assetsDirectoryPath().string(),
                [&](const std::string &path, const filewatch::Event event) { onFileSystemChange(path, event); }
            );
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

            m_assetManager->generateMetadataForAllFiles(m_project->assetsDirectoryPath());
            m_assetManager->loadMappings(m_project->assetsDirectoryPath());

            m_fileWatch = std::make_unique<filewatch::FileWatch<std::string>>(
                m_project->assetsDirectoryPath().string(),
                [&](const std::string &path, const filewatch::Event event) { onFileSystemChange(path, event); }
            );
        }
    }

    ImGui::End();
}

void Editor::onMenuBar(Project &project) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Scene")) {
            if (ImGui::MenuItem("Save as", nullptr, false, m_scene != nullptr)) {
                // TODO: Support for non-ascii characters
                NFD::UniquePath path;

                auto assetsDirectoryString = project.assetsDirectoryPath().string();

                nfdu8filteritem_t filterItem = { "Scene file", "scene" };

                if (NFD::SaveDialog(path, &filterItem, 1, assetsDirectoryString.c_str()) == NFD_OKAY) {
                    std::ofstream stream(path.get());

                    stream << m_sceneSerde.serialize(*m_scene);
                }
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Editor::onHierarchyPanel() {
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

void Editor::onViewportPanel(std::shared_ptr<Texture2D> &viewportTexture, float deltaTime) {
    ImGui::Begin("Viewport");

    auto icon = isPlaying ? m_stopIconTexture : m_playIconTexture;

    if (ImGui::ImageButton(icon->view(), { 32.0f, 32.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f })) {
        auto selectedEntityId = m_selectedEntity != nullptr ? std::make_optional(m_selectedEntity->id()) : std::nullopt;

        if (!isPlaying) {
            m_engine->setCurrentScene(std::make_shared<Scene>(Scene::copy(*m_scene)));
            m_engine->currentScene()->start();

            {
                m_scriptEngine->setup();

                m_scriptEngine->setInternalCall("DelusionSharp.Internals::IsKeyDown", &isKeyDown);

                m_scriptEngine->setInternalCall(
                    "DelusionSharp.Internals::HasTransformComponent", &hasTransformComponent
                );
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::GetTransformPosition", &getTransformPosition);
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::SetTransformPosition", &setTransformPosition);
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::GetTransformScale", &getTransformScale);
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::SetTransformScale", &setTransformScale);
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::GetTransformRotation", &getTransformRotation);
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::SetTransformRotation", &setTransformRotation);

                m_scriptEngine->setInternalCall("DelusionSharp.Internals::HasSpriteComponent", &hasSpriteComponent);
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::GetSpriteTexture", &getSpriteTexture);
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::SetSpriteTexture", &setSpriteTexture);

                m_scriptEngine->setInternalCall(
                    "DelusionSharp.Internals::HasRigidbodyComponent", &hasRigidbodyComponent
                );
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::GetRigidbodyBodyType", &getRigidbodyBodyType);
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::SetRigidbodyBodyType", &setRigidbodyBodyType);
                m_scriptEngine->setInternalCall(
                    "DelusionSharp.Internals::GetRigidbodyHasFixedRotation", &getRigidbodyHasFixedRotation
                );
                m_scriptEngine->setInternalCall(
                    "DelusionSharp.Internals::SetRigidbodyHasFixedRotation", &setRigidbodyHasFixedRotation
                );
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::GetRigidbodyDensity", &getRigidbodyDensity);
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::SetRigidbodyDensity", &setRigidbodyDensity);
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::GetRigidbodyFriction", &getRigidbodyFriction);
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::SetRigidbodyFriction", &setRigidbodyFriction);
                m_scriptEngine->setInternalCall(
                    "DelusionSharp.Internals::GetRigidbodyRestitution", &getRigidbodyRestitution
                );
                m_scriptEngine->setInternalCall(
                    "DelusionSharp.Internals::SetRigidbodyRestitution", &setRigidbodyRestitution
                );
                m_scriptEngine->setInternalCall(
                    "DelusionSharp.Internals::GetRigidbodyRestitutionThreshold", &getRigidbodyRestitutionThreshold
                );
                m_scriptEngine->setInternalCall(
                    "DelusionSharp.Internals::SetRigidbodyRestitutionThreshold", &setRigidbodyRestitutionThreshold
                );

                m_scriptEngine->setInternalCall(
                    "DelusionSharp.Internals::HasBoxColliderComponent", &hasBoxColliderComponent
                );
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::GetBoxColliderSize", &getBoxColliderSize);
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::SetBoxColliderSize", &setBoxColliderSize);
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::GetBoxColliderOffset", &getBoxColliderOffset);
                m_scriptEngine->setInternalCall("DelusionSharp.Internals::SetBoxColliderOffset", &setBoxColliderOffset);

                auto delusionSharpLibraryPath = std::filesystem::current_path() / "DelusionSharp.dll";
                auto delusionSharpLibraryPathString = delusionSharpLibraryPath.string();

                auto gameLibraryPath = m_project->assetsDirectoryPath() / "Game.dll";
                auto gameLibraryPathString = gameLibraryPath.string();

                auto delusionSharpAssembly = m_scriptEngine->loadAssembly(delusionSharpLibraryPathString);
                auto gameAssembly = m_scriptEngine->loadAssembly(gameLibraryPathString);

                auto scriptClass = delusionSharpAssembly.getClassByName("DelusionSharp", "Script");
                auto entityClass = delusionSharpAssembly.getClassByName("DelusionSharp", "Entity");

                m_engine->currentScene()->forEachEntity([this, &gameAssembly, &scriptClass,
                                                         &entityClass](Entity &entity) {
                    if (entity.hasComponent<ScriptComponent>()) {
                        auto &script = entity.getComponent<ScriptComponent>();

                        std::string scriptNamespace;
                        std::string scriptName;

                        auto dotIndex = script.name.find('.');

                        if (dotIndex != std::string::npos) {
                            scriptNamespace = script.name.substr(0, dotIndex);
                            scriptName = script.name.substr(dotIndex + 1);
                        } else {
                            scriptNamespace = "";
                            scriptName = script.name;
                        }

                        auto class_ = gameAssembly.getClassByName(scriptNamespace, scriptName);
                        auto instance = m_scriptEngine->createInstance(class_);

                        script.class_ = static_cast<void *>(class_.getMonoClass());
                        script.instance = static_cast<void *>(instance.getMonoObject());

                        auto entityInstance = m_scriptEngine->createInstance(entityClass);

                        auto entityField = scriptClass.getField("_entity");
                        auto idField = entityClass.getField("_id");

                        auto id = entity.id().value();

                        mono_field_set_value(entityInstance.getMonoObject(), idField, &id);
                        mono_field_set_value(instance.getMonoObject(), entityField, entityInstance.getMonoObject());

                        auto onCreateMethod = class_.getMethod("OnCreate", 0);

                        mono_runtime_invoke(onCreateMethod, instance.getMonoObject(), nullptr, nullptr);
                    }
                });
            }

            if (selectedEntityId.has_value()) {
                auto entity = m_engine->currentScene()->getById(selectedEntityId.value());

                if (entity.has_value()) {
                    m_selectedEntity = entity.value();
                } else {
                    m_selectedEntity = nullptr;
                }
            }
        } else {
            m_scriptEngine->teardown();

            m_engine->currentScene()->stop();
            m_engine->setCurrentScene(m_scene);

            if (selectedEntityId.has_value()) {
                auto entity = m_engine->currentScene()->getById(selectedEntityId.value());

                if (entity.has_value()) {
                    m_selectedEntity = entity.value();
                } else {
                    m_selectedEntity = nullptr;
                }
            }
        }

        isPlaying = !isPlaying;
    }

    ImVec2 availableSpace = ImGui::GetContentRegionAvail();

    m_camera.setAspectRatio(availableSpace.x / availableSpace.y);

    if (availableSpace.x > 0.0f && availableSpace.y > 0.0f) {
        auto availableWidth = static_cast<uint32_t>(availableSpace.x);
        auto availableHeight = static_cast<uint32_t>(availableSpace.y);

        if (viewportTexture->width() != availableWidth || viewportTexture->height() != availableHeight) {
            std::shared_ptr<Texture2D> newTexture =
                Texture2D::create(viewportTexture->id(), m_device, availableWidth, availableHeight, true);

            viewportTexture.swap(newTexture);
        }
    }

    ImGui::Image(viewportTexture->view(), availableSpace);

    ImGui::SetItemUsingMouseWheel();

    if (ImGui::IsWindowFocused()) {
        auto dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);

        glm::vec2 movement =
            glm::vec2(dragDelta.x * m_camera.aspectRatio() / availableSpace.x, dragDelta.y / availableSpace.y) *
            glm::vec2(-1.0f, 1.0f) * m_camera.zoom() * deltaTime * 125.0f;

        m_camera.setPosition(m_camera.position() + glm::vec3(movement, 0.0f));

        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);

        auto mouseWheelTurns = ImGui::GetIO().MouseWheel;

        if (mouseWheelTurns > 0.0f) {
            m_camera.setZoom(max(m_camera.zoom() * 0.50f, 0.015625f));
        } else if (mouseWheelTurns < 0.0f) {
            m_camera.setZoom(min(m_camera.zoom() * 2.0f, 64.0f));
        }
    }

    if (ImGui::BeginDragDropTarget()) {
        auto payload = ImGui::AcceptDragDropPayload("scene");

        if (payload != nullptr) {
            auto source = readAsString(static_cast<char *>(payload->Data));

            if (source.has_value()) {
                m_selectedEntity = nullptr;

                if (m_engine->currentScene() != nullptr) {
                    m_scene->stop();
                    isPlaying = false;
                }

                m_scene = std::make_shared<Scene>(m_sceneSerde.deserialize(source.value()));
                m_engine->setCurrentScene(m_scene);
            }
        }

        ImGui::EndDragDropTarget();
    }

    ImGui::End();
}

void Editor::onAssetBrowserPanel(Project &project) {
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

    for (const auto &entry : std::filesystem::directory_iterator(m_currentDirectory)) {
        const auto &path = entry.path();

        auto extension = path.filename().extension();
        auto extensionText = extension.string();

        if (extensionText == ".metadata") {
            continue;
        }

        // TODO: Support for non-ascii characters
        std::string name = path.filename().string();

        auto iconTexture = entry.is_directory() ? m_directoryIconTexture : m_fileIconTexture;

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
        if (m_selectedEntity->hasComponent<TransformComponent>()) {
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::BeginPopupContextItem(nullptr)) {
                    if (ImGui::MenuItem("Remove component")) {
                        m_selectedEntity->removeComponent<TransformComponent>();
                    }

                    ImGui::EndPopup();
                }

                if (m_selectedEntity->hasComponent<TransformComponent>()) {
                    auto &transform = m_selectedEntity->getComponent<TransformComponent>();

                    ImGui::DragFloat2("Position", glm::value_ptr(transform.position), 0.1f, 0.0f, 0.0f, "%.5f");
                    ImGui::DragFloat("Rotation", &transform.rotation, 0.1f, 0.0f, 0.0f, "%.5f");
                    ImGui::DragFloat2("Scale", glm::value_ptr(transform.scale), 0.1f, 0.0f, 0.0f, "%.5f");
                }
            }
        }

        if (m_selectedEntity->hasComponent<SpriteComponent>()) {
            if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::BeginPopupContextItem(nullptr)) {
                    if (ImGui::MenuItem("Remove component")) {
                        m_selectedEntity->removeComponent<SpriteComponent>();
                    }

                    ImGui::EndPopup();
                }

                if (m_selectedEntity->hasComponent<SpriteComponent>()) {
                    auto &sprite = m_selectedEntity->getComponent<SpriteComponent>();

                    if (sprite.texture != nullptr) {
                        ImGui::Image(sprite.texture->view(), ImVec2(128.0f, 128.0f), ImVec2(0, 1), ImVec2(1, 0));
                    } else {
                        ImGui::Image(m_emptyTexture->view(), ImVec2(128.0f, 128.0f), ImVec2(0, 1), ImVec2(1, 0));
                    }

                    if (ImGui::BeginDragDropTarget()) {
                        auto payload = ImGui::AcceptDragDropPayload("image");

                        if (payload != nullptr) {
                            auto path = std::filesystem::path(static_cast<char *>(payload->Data));

                            if (!m_assetManager->isLoaded(path)) {
                                m_assetManager->loadAsset(path);
                            }

                            auto texture = m_assetManager->getTextureById(m_assetManager->getIdByPath(path));

                            sprite.texture = texture;
                        }

                        ImGui::EndDragDropTarget();
                    }
                }
            }
        }

        if (m_selectedEntity->hasComponent<RigidbodyComponent>()) {
            if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::BeginPopupContextItem(nullptr)) {
                    if (ImGui::MenuItem("Remove component")) {
                        m_selectedEntity->removeComponent<RigidbodyComponent>();
                    }

                    ImGui::EndPopup();
                }

                if (m_selectedEntity->hasComponent<RigidbodyComponent>()) {
                    auto &rigidbody = m_selectedEntity->getComponent<RigidbodyComponent>();

                    static const char *types[] = {
                        "static",
                        "dynamic",
                        "kinematic",
                    };

                    int currentItem {};

                    switch (rigidbody.bodyType) {
                        case RigidbodyComponent::BodyType::Static:
                            currentItem = 0;

                            break;
                        case RigidbodyComponent::BodyType::Dynamic:
                            currentItem = 1;

                            break;
                        case RigidbodyComponent::BodyType::Kinematic:
                            currentItem = 2;

                            break;
                    }

                    if (ImGui::Combo("Body type", &currentItem, types, 3)) {
                        switch (currentItem) {
                            case 0:
                                rigidbody.bodyType = RigidbodyComponent::BodyType::Static;

                                break;
                            case 1:
                                rigidbody.bodyType = RigidbodyComponent::BodyType::Dynamic;

                                break;
                            case 2:
                                rigidbody.bodyType = RigidbodyComponent::BodyType::Kinematic;

                                break;
                            default:
                                assert(false);
                        }
                    }

                    ImGui::Checkbox("Has fixed rotation", &rigidbody.hasFixedRotation);

                    ImGui::DragFloat("Density", &rigidbody.density, 0.1f, 0.0f, 0.0f, "%.5f");
                    ImGui::DragFloat("Friction", &rigidbody.friction, 0.1f, 0.0f, 0.0f, "%.5f");
                    ImGui::DragFloat("Restitution", &rigidbody.restitution, 0.1f, 0.0f, 0.0f, "%.5f");
                    ImGui::DragFloat(
                        "Restitution threshold", &rigidbody.restitutionThreshold, 0.1f, 0.0f, 0.0f, "%.5f"
                    );
                }
            }
        }

        if (m_selectedEntity->hasComponent<BoxColliderComponent>()) {
            if (ImGui::CollapsingHeader("Box collider", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::BeginPopupContextItem(nullptr)) {
                    if (ImGui::MenuItem("Remove component")) {
                        m_selectedEntity->removeComponent<BoxColliderComponent>();
                    }

                    ImGui::EndPopup();
                }

                if (m_selectedEntity->hasComponent<BoxColliderComponent>()) {
                    auto &collider = m_selectedEntity->getComponent<BoxColliderComponent>();

                    ImGui::DragFloat2("Size", glm::value_ptr(collider.size), 0.1f, 0.0f, 0.0f, "%.5f");
                    ImGui::DragFloat2("Offset", glm::value_ptr(collider.offset), 0.1f, 0.0f, 0.0f, "%.5f");
                }
            }
        }

        if (m_selectedEntity->hasComponent<ScriptComponent>()) {
            if (ImGui::CollapsingHeader("Script", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::BeginPopupContextItem(nullptr)) {
                    if (ImGui::MenuItem("Remove component")) {
                        m_selectedEntity->removeComponent<ScriptComponent>();
                    }

                    ImGui::EndPopup();
                }

                if (m_selectedEntity->hasComponent<ScriptComponent>()) {
                    auto &script = m_selectedEntity->getComponent<ScriptComponent>();

                    std::array<char, 256> nameBuffer {};

                    std::copy(script.name.begin(), script.name.end(), nameBuffer.begin());

                    ImGui::InputText("Name", nameBuffer.data(), nameBuffer.size());

                    script.name = std::string(nameBuffer.data());

                    if (isPlaying) {
                        ImGui::Separator();

                        CSharpClass scriptClass(static_cast<MonoClass *>(script.class_));

                        for (auto &field : scriptClass.getFields()) {
                            if (!field.isPublic()) {
                                continue;
                            }

                            if (field.isStatic()) {
                                continue;
                            }

                            auto type = field.getType();
                            auto engineType = type.asEngineType();

                            if (engineType.has_value()) {
                                auto engineTypeValue = engineType.value();

                                CSharpObject object(static_cast<MonoObject *>(script.instance));

                                switch (engineTypeValue) {
                                    case Boolean: {
                                        auto value = field.getValue<bool>(object);

                                        ImGui::Checkbox(field.getRawName(), &value);

                                        field.setValue<bool>(object, value);

                                        break;
                                    }
                                    case Char:
                                        // TODO

                                        break;
                                    case Byte: {
                                        auto value = field.getValue<uint8_t>(object);

                                        ImGui::InputScalar(field.getRawName(), ImGuiDataType_U8, &value);

                                        field.setValue<uint8_t>(object, value);

                                        break;
                                    }
                                    case SByte: {
                                        auto value = field.getValue<int8_t>(object);

                                        ImGui::InputScalar(field.getRawName(), ImGuiDataType_S8, &value);

                                        field.setValue<int8_t>(object, value);

                                        break;
                                    }
                                    case Int16: {
                                        auto value = field.getValue<int16_t>(object);

                                        ImGui::InputScalar(field.getRawName(), ImGuiDataType_S16, &value);

                                        field.setValue<int16_t>(object, value);

                                        break;
                                    }
                                    case UInt16: {
                                        auto value = field.getValue<uint16_t>(object);

                                        ImGui::InputScalar(field.getRawName(), ImGuiDataType_U16, &value);

                                        field.setValue<uint16_t>(object, value);

                                        break;
                                    }
                                    case Int32: {
                                        auto value = field.getValue<int32_t>(object);

                                        ImGui::InputScalar(field.getRawName(), ImGuiDataType_S32, &value);

                                        field.setValue<int32_t>(object, value);

                                        break;
                                    }
                                    case UInt32: {
                                        auto value = field.getValue<uint32_t>(object);

                                        ImGui::InputScalar(field.getRawName(), ImGuiDataType_U32, &value);

                                        field.setValue<uint32_t>(object, value);

                                        break;
                                    }
                                    case Int64: {
                                        auto value = field.getValue<int64_t>(object);

                                        ImGui::InputScalar(field.getRawName(), ImGuiDataType_S64, &value);

                                        field.setValue<int64_t>(object, value);

                                        break;
                                    }
                                    case UInt64: {
                                        auto value = field.getValue<uint64_t>(object);

                                        ImGui::InputScalar(field.getRawName(), ImGuiDataType_U64, &value);

                                        field.setValue<uint64_t>(object, value);

                                        break;
                                    }
                                    case Float: {
                                        auto value = field.getValue<float>(object);

                                        ImGui::InputFloat(field.getRawName(), &value);

                                        field.setValue<float>(object, value);

                                        break;
                                    }
                                    case Double: {
                                        auto value = field.getValue<double>(object);

                                        ImGui::InputDouble(field.getRawName(), &value);

                                        field.setValue<double>(object, value);

                                        break;
                                    }
                                    case Vector2: {
                                        auto value = field.getValue<glm::vec2>(object);

                                        ImGui::DragFloat2(
                                            field.getRawName(), glm::value_ptr(value), 0.1f, 0.0f, 0.0f, "%.5f"
                                        );

                                        field.setValue<glm::vec2>(object, value);

                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (ImGui::Button("Add component")) {
            ImGui::OpenPopup("add_component_popup");
        }

        if (ImGui::BeginPopup("add_component_popup")) {
            if (!m_selectedEntity->hasComponent<TransformComponent>()) {
                if (ImGui::Button("Transform")) {
                    m_selectedEntity->addComponent<TransformComponent>();

                    ImGui::CloseCurrentPopup();
                }
            }

            if (!m_selectedEntity->hasComponent<SpriteComponent>()) {
                if (ImGui::Button("Sprite")) {
                    m_selectedEntity->addComponent<SpriteComponent>();

                    ImGui::CloseCurrentPopup();
                }
            }

            if (!m_selectedEntity->hasComponent<RigidbodyComponent>()) {
                if (ImGui::Button("Rigidbody")) {
                    m_selectedEntity->addComponent<RigidbodyComponent>();

                    ImGui::CloseCurrentPopup();
                }
            }

            if (!m_selectedEntity->hasComponent<BoxColliderComponent>()) {
                if (ImGui::Button("Box collider")) {
                    m_selectedEntity->addComponent<BoxColliderComponent>();

                    ImGui::CloseCurrentPopup();
                }
            }

            if (!m_selectedEntity->hasComponent<ScriptComponent>()) {
                if (ImGui::Button("Script")) {
                    m_selectedEntity->addComponent<ScriptComponent>();

                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }
    }

    ImGui::End();
}

bool Editor::entityHierarchy(Entity &entity) {
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

void Editor::onFileSystemChange(const std::string &relativePath, const filewatch::Event event) {
    std::filesystem::path path = m_project->assetsDirectoryPath() / relativePath;

    switch (event) {
        case filewatch::Event::added:
            m_assetManager->generateMetadataForFile(path);
            m_assetManager->loadMapping(path);

            break;
        case filewatch::Event::removed:
            m_assetManager->unloadMapping(m_assetManager->getIdByPath(path));
            m_assetManager->deleteMetadataOfFile(path);

            break;
        case filewatch::Event::renamed_old:
            m_fileBeingRenamed = std::make_optional(path);

            break;
        case filewatch::Event::renamed_new:
            m_assetManager->updateMapping(m_assetManager->getIdByPath(m_fileBeingRenamed.value()), path);
            m_assetManager->renameMetadataOfFile(m_fileBeingRenamed.value(), path);

            m_fileBeingRenamed = std::nullopt;

            break;
        default:
            break;
    };
}
