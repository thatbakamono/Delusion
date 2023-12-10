#pragma once

#include <yaml-cpp/yaml.h>

#include <utility>

#include "delusion/AssetManager.hpp"
#include "delusion/Scene.hpp"

class SceneSerde {
    private:
        std::shared_ptr<AssetManager> m_assetManager;
    public:
        explicit SceneSerde(std::shared_ptr<AssetManager> assetManager) : m_assetManager(std::move(assetManager)) {}

        [[nodiscard]] Scene deserialize(const std::string &input);

        [[nodiscard]] std::string serialize(const Scene &scene);
    private:
        void deserializeEntity(YAML::Node &entityNode, Entity &entity);

        void serializeEntity(YAML::Emitter &emitter, const Entity &entity);
};
