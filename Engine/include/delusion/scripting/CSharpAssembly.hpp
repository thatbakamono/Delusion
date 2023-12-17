#pragma once

#include <string>

#include <mono/metadata/assembly.h>
#include <mono/metadata/image.h>

#include "delusion/scripting/CSharpClass.hpp"

class CSharpAssembly {
    private:
        MonoAssembly *m_assembly {};
        MonoImage *m_image {};
    public:
        CSharpAssembly(MonoAssembly *assembly, MonoImage *image) : m_assembly(assembly), m_image(image) {}

        [[nodiscard]] CSharpClass getClassByName(const std::string &_namespace, const std::string &name) {
            return CSharpClass(mono_class_from_name(m_image, _namespace.c_str(), name.c_str()));
        }
};
