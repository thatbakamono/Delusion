#pragma once

#include <yaml-cpp/yaml.h>

#include "delusion/Metadata.hpp"

class MetadataSerde {
    public:
        [[nodiscard]] static Metadata deserialize(const std::string &input);

        [[nodiscard]] static std::string serialize(const Metadata &metadata);
};
