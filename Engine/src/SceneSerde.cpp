#include "delusion/SceneSerde.hpp"

#include "delusion/Components.hpp"

Scene SceneSerde::deserialize(const std::string &input) {
    Scene scene;

    YAML::Node node = YAML::Load(input);
    auto entitiesNode = node["entities"];

    for (size_t entityIndex = 0; entityIndex < entitiesNode.size(); entityIndex++) {
        auto entityNode = entitiesNode[entityIndex];

        auto& entity = scene.create();

        deserializeEntity(entityNode, entity);
    }

    return scene;
}

std::string SceneSerde::serialize(const Scene &scene) {
    YAML::Emitter emitter;

    emitter << YAML::BeginMap;

    emitter << YAML::Key << "entities";
    emitter << YAML::BeginSeq;

    for (const auto& entity : scene.entities()) {
        serializeEntity(emitter, entity);
    }

    emitter << YAML::EndSeq;

    emitter << YAML::EndMap;

    return { emitter.c_str() };
}

void SceneSerde::deserializeEntity(YAML::Node &entityNode, Entity &entity) {
    // TODO: Implement some kind of component registry with metadata, so it doesn't have to be done manually
    auto componentsNode = entityNode["components"];

    if (componentsNode) {
        auto transformNode = componentsNode["transform"];

        if (transformNode) {
            auto positionNode = transformNode["position"];
            auto rotationNode = transformNode["rotation"];
            auto scaleNode = transformNode["scale"];

            glm::vec2 position(positionNode["x"].as<float>(), positionNode["y"].as<float>());
            auto rotation = rotationNode.as<float>();
            glm::vec2 scale(scaleNode["width"].as<float>(), scaleNode["height"].as<float>());

            entity.addComponent<Transform>(position, scale, rotation);
        }
    }

    auto childrenNode = entityNode["children"];

    if (childrenNode) {
        for (size_t childIndex = 0; childIndex < childrenNode.size(); childIndex++) {
            auto& childEntity = entity.createChild();
            auto childNode = childrenNode[childIndex];

            deserializeEntity(childNode, childEntity);
        }
    }
}

void SceneSerde::serializeEntity(YAML::Emitter &emitter, const Entity &entity) {
    emitter << YAML::BeginMap;

    // TODO: Implement some kind of component registry with metadata, so it doesn't have to be done manually
    if (entity.hasComponent<Transform>()) {
        emitter << YAML::Key << "components";
        emitter << YAML::BeginMap;

        const auto& transform = entity.getComponent<Transform>();

        emitter << YAML::Key << "transform";
        emitter << YAML::BeginMap;

        emitter << YAML::Key << "position";
        emitter << YAML::BeginMap;

        emitter << YAML::Key << "x";
        emitter << YAML::Value << transform.position.x;

        emitter << YAML::Key << "y";
        emitter << YAML::Value << transform.position.y;

        emitter << YAML::EndMap;

        emitter << YAML::Key << "rotation";
        emitter << YAML::Value << transform.rotation;

        emitter << YAML::Key << "scale";
        emitter << YAML::BeginMap;

        emitter << YAML::Key << "width";
        emitter << YAML::Value << transform.scale.x;

        emitter << YAML::Key << "height";
        emitter << YAML::Value << transform.scale.y;

        emitter << YAML::EndMap;

        emitter << YAML::EndMap;

        emitter << YAML::EndMap;
    }

    emitter << YAML::Key << "children";
    emitter << YAML::BeginSeq;

    for (const auto& child : entity.children()) {
        serializeEntity(emitter, child);
    }

    emitter << YAML::EndSeq;

    emitter << YAML::EndMap;
}
