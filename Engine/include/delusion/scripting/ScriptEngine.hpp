#pragma once

#include <string>
#include <memory>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/threads.h>

#include "delusion/scripting/CSharpAssembly.hpp"
#include "delusion/scripting/CSharpDomain.hpp"
#include "delusion/scripting/CSharpObject.hpp"

class ScriptEngine {
    private:
        std::unique_ptr<MonoDomain, decltype(&mono_jit_cleanup)> m_rootDomain { nullptr, mono_jit_cleanup };

        MonoDomain *m_domain;
    public:
        ScriptEngine() {
            mono_set_assemblies_path("mono/4.5");

            m_rootDomain = std::unique_ptr<MonoDomain, decltype(&mono_jit_cleanup)>(mono_jit_init("delusion"), mono_jit_cleanup);

            if (m_rootDomain == nullptr) {
                throw std::exception();
            }

            mono_thread_set_main(mono_thread_current());
        }

        void setup() {
            m_domain = mono_domain_create_appdomain(nullptr, nullptr);

            mono_domain_set(m_domain, true);
        }

        void teardown() {
            mono_domain_set(m_rootDomain.get(), true);

            mono_domain_unload(m_domain);
        }

        [[nodiscard]] CSharpAssembly loadAssembly(const std::string &path) {
            auto *assembly = mono_domain_assembly_open(m_domain, path.c_str());
            auto *image = mono_assembly_get_image(assembly);

            return { assembly, image };
        }

        CSharpObject createInstance(CSharpClass csharpClass) {
            MonoObject *object = mono_object_new(m_domain, csharpClass.getMonoClass());

            mono_runtime_object_init(object);

            return CSharpObject(object);
        }

        template<typename Function>
        void setInternalCall(const std::string &name, Function function) {
            mono_add_internal_call(name.c_str(), function);
        }
};
