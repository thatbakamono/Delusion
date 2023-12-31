#pragma once

#include <mono/metadata/class.h>
#include <mono/metadata/tabledefs.h>

#include "delusion/scripting/CSharpObject.hpp"
#include "delusion/scripting/CSharpType.hpp"

class CSharpField {
    private:
        MonoClassField *m_field;
        CSharpType m_type;
        uint32_t m_flags;
    public:
        explicit CSharpField(MonoClassField *field)
            : m_field(field), m_type(CSharpType(mono_field_get_type(m_field))), m_flags(mono_field_get_flags(m_field)) {
        }

        [[nodiscard]] std::string getName() const {
            return mono_field_get_name(m_field);
        }

        [[nodiscard]] const char *getRawName() const {
            return mono_field_get_name(m_field);
        }

        [[nodiscard]] bool isPublic() const {
            return m_flags & FIELD_ATTRIBUTE_PUBLIC;
        }

        [[nodiscard]] bool isStatic() const {
            return m_flags & FIELD_ATTRIBUTE_STATIC;
        }

        [[nodiscard]] CSharpType getType() {
            return m_type;
        }

        template <typename T>
        [[nodiscard]] T getValue(CSharpObject &object) {
            T value {};

            mono_field_get_value(object.getMonoObject(), m_field, &value);

            return value;
        }

        template <typename T>
        void setValue(CSharpObject &object, T value) {
            mono_field_set_value(object.getMonoObject(), m_field, &value);
        }
};
