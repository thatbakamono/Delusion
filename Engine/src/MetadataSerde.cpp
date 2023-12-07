#include "delusion/MetadataSerde.hpp"

Metadata MetadataSerde::deserialize(const std::string &input) {
    YAML::Node node = YAML::Load(input);

    return Metadata { UniqueId(node["unique_id"].as<uint64_t>()) };
}

std::string MetadataSerde::serialize(const Metadata &metadata) {
    YAML::Emitter emitter;

    emitter << YAML::BeginMap;

    emitter << YAML::Key << "unique_id";
    emitter << YAML::Value << metadata.id.value();

    emitter << YAML::EndMap;

    return { emitter.c_str() };
}
