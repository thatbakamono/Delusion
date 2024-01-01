#include "editor/ui/ViewportPanel.hpp"

#include <utility>

#include <imgui.h>
#include <imgui_internal.h>

#include "editor/Editor.hpp"

#include <delusion/scripting/Internals.hpp>
#include <delusion/Utilities.hpp>

ViewportPanel::ViewportPanel(
    Editor &editor, HierarchyPanel &hierarchyPanel, Engine *engine, std::shared_ptr<ScriptEngine> scriptEngine,
    std::shared_ptr<Texture2D> playIconTexture, std::shared_ptr<Texture2D> stopIconTexture
)
    : m_editor(editor), m_hierarchyPanel(hierarchyPanel), m_engine(engine), m_scriptEngine(std::move(scriptEngine)),
      m_playIconTexture(std::move(playIconTexture)), m_stopIconTexture(std::move(stopIconTexture)),
      m_sceneSerde(engine->assetManager()) {}

void ViewportPanel::onUpdate(Project &project, std::shared_ptr<Texture2D> &viewportTexture, float deltaTime) {
    ImGui::Begin("Viewport");

    auto isPlaying = m_editor.isPlaying();

    auto icon = isPlaying ? m_stopIconTexture : m_playIconTexture;

    if (ImGui::ImageButton(icon->view(), { 32.0f, 32.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f })) {
        auto selectedEntity = m_hierarchyPanel.selectedEntity();

        auto selectedEntityId = selectedEntity != nullptr ? std::make_optional(selectedEntity->id()) : std::nullopt;

        if (!isPlaying) {
            m_engine->setCurrentScene(std::make_shared<Scene>(Scene::copy(*m_editor.scene())));
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

                auto gameLibraryPath = project.assetsDirectoryPath() / "Game.dll";
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
                    m_hierarchyPanel.setSelectedEntity(entity.value());
                } else {
                    m_hierarchyPanel.setSelectedEntity(nullptr);
                }
            }
        } else {
            m_scriptEngine->teardown();

            m_engine->currentScene()->stop();
            m_engine->setCurrentScene(m_editor.scene());

            if (selectedEntityId.has_value()) {
                auto entity = m_engine->currentScene()->getById(selectedEntityId.value());

                if (entity.has_value()) {
                    m_hierarchyPanel.setSelectedEntity(entity.value());
                } else {
                    m_hierarchyPanel.setSelectedEntity(nullptr);
                }
            }
        }

        m_editor.setPlaying(!isPlaying);
    }

    ImVec2 availableSpace = ImGui::GetContentRegionAvail();

    m_camera.setAspectRatio(availableSpace.x / availableSpace.y);

    if (availableSpace.x > 0.0f && availableSpace.y > 0.0f) {
        auto availableWidth = static_cast<uint32_t>(availableSpace.x);
        auto availableHeight = static_cast<uint32_t>(availableSpace.y);

        if (viewportTexture->width() != availableWidth || viewportTexture->height() != availableHeight) {
            std::shared_ptr<Texture2D> newTexture = Texture2D::create(
                viewportTexture->id(), m_engine->graphicsBackend()->device(), availableWidth, availableHeight, true
            );

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
                m_hierarchyPanel.setSelectedEntity(nullptr);

                if (m_engine->currentScene() != nullptr) {
                    m_editor.scene()->stop();
                    isPlaying = false;
                }

                m_editor.setScene(std::make_shared<Scene>(m_sceneSerde.deserialize(source.value())));
                m_engine->setCurrentScene(m_editor.scene());
            }
        }

        ImGui::EndDragDropTarget();
    }

    ImGui::End();
}
