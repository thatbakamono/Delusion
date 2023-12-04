#include "delusion/SceneSerde.hpp"

Scene SceneSerde::deserialize(const std::string &input) {
    Scene scene;

    YAML::Node node = YAML::Load(input);
    auto entitiesNode = node["entities"];

    for (size_t entityIndex = 0; entityIndex < entitiesNode.size(); entityIndex++) {
        auto entityNode = entitiesNode[entityIndex];

        auto& entity = scene.create();

        auto childrenNode = entityNode["children"];

        if (childrenNode) {
            for (size_t childIndex = 0; childIndex < childrenNode.size(); childIndex++) {
                auto child = childrenNode[childIndex];

                deserializeEntity(child, entity);
            }
        }
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

void SceneSerde::deserializeEntity(YAML::Node &entityNode, Entity &parentEntity) {
    auto childrenNode = entityNode["children"];

    if (childrenNode) {
        auto& childEntity = parentEntity.createChild();

        for (size_t childIndex = 0; childIndex < childrenNode.size(); childIndex++) {
            auto childNode = childrenNode[childIndex];

            deserializeEntity(childNode, childEntity);
        }
    }
}

void SceneSerde::serializeEntity(YAML::Emitter &emitter, const Entity &entity) {
    emitter << YAML::BeginMap;

    emitter << YAML::Key << "children";
    emitter << YAML::BeginSeq;

    for (const auto& child : entity.children()) {
        serializeEntity(emitter, child);
    }

    emitter << YAML::EndSeq;

    emitter << YAML::EndMap;
}
