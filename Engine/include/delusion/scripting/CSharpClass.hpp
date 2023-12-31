#pragma once

#include <string>

#include <mono/metadata/class.h>

#include "delusion/scripting/CSharpField.hpp"

class CSharpClass {
    private:
        MonoClass *m_class {};
        std::vector<CSharpField> m_fields;
    public:
        explicit CSharpClass(MonoClass *_class) : m_class(_class) {
            void *iterator = nullptr;

            while (MonoClassField *field = mono_class_get_fields(m_class, &iterator)) {
                m_fields.emplace_back(field);
            }
        }

        [[nodiscard]] MonoMethod *getMethod(const std::string &name, int parameterCount) const {
            return mono_class_get_method_from_name(m_class, name.c_str(), parameterCount);
        }

        [[nodiscard]] MonoClassField *getField(const std::string &name) const {
            return mono_class_get_field_from_name(m_class, name.c_str());
        }

        [[nodiscard]] std::span<CSharpField> getFields() {
            return { m_fields.data(), m_fields.size() };
        }

        [[nodiscard]] MonoClass *getMonoClass() const {
            return m_class;
        }
};
