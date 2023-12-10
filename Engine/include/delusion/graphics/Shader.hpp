#pragma once

#include <memory>
#include <string>

#include <webgpu.h>

class Shader {
    private:
        WGPUShaderModule m_shaderModule;

        explicit Shader(WGPUShaderModule shaderModule) : m_shaderModule(shaderModule) {}
    public:
        Shader(const Shader &other) = delete;
        Shader(Shader &&other) noexcept = delete;

        Shader &operator=(const Shader &other) = delete;
        Shader &operator=(Shader &&other) noexcept = delete;

        [[nodiscard]] static std::unique_ptr<Shader> create(WGPUDevice device, const std::string &source);

        [[nodiscard]] static std::unique_ptr<Shader> createFromFile(WGPUDevice device, const std::string &path);

        [[nodiscard]] WGPUShaderModule shaderModule() const {
            return m_shaderModule;
        }
};
