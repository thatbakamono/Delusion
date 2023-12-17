#pragma once

#include <string>

#include <mono/metadata/class.h>

class CSharpClass {
    private:
        MonoClass *m_class {};
    public:
        explicit CSharpClass(MonoClass *_class) : m_class(_class) {}

        [[nodiscard]] MonoMethod *getMethod(const std::string &name, int parameterCount) const {
            return mono_class_get_method_from_name(m_class, name.c_str(), parameterCount);
        }

        [[nodiscard]] MonoClassField *getField(const std::string &name) const {
            return mono_class_get_field_from_name(m_class, name.c_str());
        }

        [[nodiscard]] MonoClass *getMonoClass() const {
            return m_class;
        }
};
