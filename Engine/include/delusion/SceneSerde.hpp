#pragma once

#include <yaml-cpp/yaml.h>

#include "delusion/Scene.hpp"

class SceneSerde {
public:
    [[nodiscard]] static Scene deserialize(const std::string& input);

    [[nodiscard]] static std::string serialize(const Scene& scene);
private:
    static void deserializeEntity(YAML::Node& entityNode, Entity& entity);

    static void serializeEntity(YAML::Emitter& emitter, const Entity& entity);
};
