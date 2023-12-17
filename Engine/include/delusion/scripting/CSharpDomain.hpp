#pragma once

#include <mono/jit/jit.h>

class CSharpDomain {
    private:
        MonoDomain *m_domain {};
    public:
        explicit CSharpDomain(MonoDomain *domain) : m_domain(domain) {}

        ~CSharpDomain() {
            mono_domain_unload(m_domain);
        }

        [[nodiscard]] CSharpAssembly loadAssembly(const std::string &path) {
            auto *assembly = mono_domain_assembly_open(m_domain, path.c_str());
            auto *image = mono_assembly_get_image(assembly);

            return { assembly, image };
        }
};
