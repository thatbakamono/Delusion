#pragma once

#include <mono/metadata/object.h>

class CSharpObject {
    private:
        MonoObject *m_object {};
    public:
        explicit CSharpObject(MonoObject *object) : m_object(object) {}

        [[nodiscard]] MonoObject *getMonoObject() const {
            return m_object;
        }
};
