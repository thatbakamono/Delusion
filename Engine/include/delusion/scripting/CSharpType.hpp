#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include <mono/metadata/class.h>

#include "delusion/scripting/EngineType.hpp"

class CSharpType {
    private:
        MonoType *m_type;
    public:
        explicit CSharpType(MonoType *type) : m_type(type) {}

        [[nodiscard]] std::string getName() {
            return mono_type_get_name(m_type);
        }

        [[nodiscard]] const char *getRawName() {
            return mono_type_get_name(m_type);
        }

        [[nodiscard]] std::optional<EngineType> asEngineType() {
            static std::unordered_map<std::string, EngineType> mappings = {
                { "System.Boolean", EngineType::Boolean },       { "System.Byte", EngineType::Byte },
                { "System.SByte", EngineType::SByte },           { "System.Char", EngineType::Char },
                { "System.Int16", EngineType::Int16 },           { "System.UInt16", EngineType::UInt16 },
                { "System.Int32", EngineType::Int32 },           { "System.UInt32", EngineType::UInt32 },
                { "System.Int64", EngineType::Int64 },           { "System.UInt64", EngineType::UInt64 },
                { "System.Single", EngineType::Float },          { "System.Double", EngineType::Double },
                { "DelusionSharp.Vector2", EngineType::Vector2 }
            };

            auto name = getName();

            if (mappings.contains(name)) {
                return std::make_optional(mappings.at(name));
            } else {
                return std::nullopt;
            }
        }
};
