#include "editor/ui/PropertiesPanel.hpp"

#include "editor/Editor.hpp"

void PropertiesPanel::onUpdate() {
    ImGui::Begin("Properties");

    const auto selectedEntity = m_hierarchyPanel.selectedEntity();

    if (selectedEntity != nullptr) {
        if (selectedEntity->hasComponent<TransformComponent>()) {
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::BeginPopupContextItem(nullptr)) {
                    if (ImGui::MenuItem("Remove component")) {
                        selectedEntity->removeComponent<TransformComponent>();
                    }

                    ImGui::EndPopup();
                }

                if (selectedEntity->hasComponent<TransformComponent>()) {
                    auto &transform = selectedEntity->getComponent<TransformComponent>();

                    ImGui::DragFloat2("Position", glm::value_ptr(transform.position), 0.1f, 0.0f, 0.0f, "%.5f");
                    ImGui::DragFloat("Rotation", &transform.rotation, 0.1f, 0.0f, 0.0f, "%.5f");
                    ImGui::DragFloat2("Scale", glm::value_ptr(transform.scale), 0.1f, 0.0f, 0.0f, "%.5f");
                }
            }
        }

        if (selectedEntity->hasComponent<SpriteComponent>()) {
            if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::BeginPopupContextItem(nullptr)) {
                    if (ImGui::MenuItem("Remove component")) {
                        selectedEntity->removeComponent<SpriteComponent>();
                    }

                    ImGui::EndPopup();
                }

                if (selectedEntity->hasComponent<SpriteComponent>()) {
                    auto &sprite = selectedEntity->getComponent<SpriteComponent>();

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

        if (selectedEntity->hasComponent<RigidbodyComponent>()) {
            if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::BeginPopupContextItem(nullptr)) {
                    if (ImGui::MenuItem("Remove component")) {
                        selectedEntity->removeComponent<RigidbodyComponent>();
                    }

                    ImGui::EndPopup();
                }

                if (selectedEntity->hasComponent<RigidbodyComponent>()) {
                    auto &rigidbody = selectedEntity->getComponent<RigidbodyComponent>();

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

        if (selectedEntity->hasComponent<BoxColliderComponent>()) {
            if (ImGui::CollapsingHeader("Box collider", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::BeginPopupContextItem(nullptr)) {
                    if (ImGui::MenuItem("Remove component")) {
                        selectedEntity->removeComponent<BoxColliderComponent>();
                    }

                    ImGui::EndPopup();
                }

                if (selectedEntity->hasComponent<BoxColliderComponent>()) {
                    auto &collider = selectedEntity->getComponent<BoxColliderComponent>();

                    ImGui::DragFloat2("Size", glm::value_ptr(collider.size), 0.1f, 0.0f, 0.0f, "%.5f");
                    ImGui::DragFloat2("Offset", glm::value_ptr(collider.offset), 0.1f, 0.0f, 0.0f, "%.5f");
                }
            }
        }

        if (selectedEntity->hasComponent<ScriptComponent>()) {
            if (ImGui::CollapsingHeader("Script", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::BeginPopupContextItem(nullptr)) {
                    if (ImGui::MenuItem("Remove component")) {
                        selectedEntity->removeComponent<ScriptComponent>();
                    }

                    ImGui::EndPopup();
                }

                if (selectedEntity->hasComponent<ScriptComponent>()) {
                    auto &script = selectedEntity->getComponent<ScriptComponent>();

                    std::array<char, 256> nameBuffer {};

                    std::copy(script.name.begin(), script.name.end(), nameBuffer.begin());

                    ImGui::InputText("Name", nameBuffer.data(), nameBuffer.size());

                    script.name = std::string(nameBuffer.data());

                    if (editor.isPlaying()) {
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
            if (!selectedEntity->hasComponent<TransformComponent>()) {
                if (ImGui::Button("Transform")) {
                    selectedEntity->addComponent<TransformComponent>();

                    ImGui::CloseCurrentPopup();
                }
            }

            if (!selectedEntity->hasComponent<SpriteComponent>()) {
                if (ImGui::Button("Sprite")) {
                    selectedEntity->addComponent<SpriteComponent>();

                    ImGui::CloseCurrentPopup();
                }
            }

            if (!selectedEntity->hasComponent<RigidbodyComponent>()) {
                if (ImGui::Button("Rigidbody")) {
                    selectedEntity->addComponent<RigidbodyComponent>();

                    ImGui::CloseCurrentPopup();
                }
            }

            if (!selectedEntity->hasComponent<BoxColliderComponent>()) {
                if (ImGui::Button("Box collider")) {
                    selectedEntity->addComponent<BoxColliderComponent>();

                    ImGui::CloseCurrentPopup();
                }
            }

            if (!selectedEntity->hasComponent<ScriptComponent>()) {
                if (ImGui::Button("Script")) {
                    selectedEntity->addComponent<ScriptComponent>();

                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }
    }

    ImGui::End();
}
