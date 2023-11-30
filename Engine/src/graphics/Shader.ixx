module;

#include <fstream>
#include <string>
#include <memory>

#include <webgpu.h>

export module shader;

export class Shader {
private:
    WGPUShaderModule m_shaderModule;

    explicit Shader(WGPUShaderModule shaderModule) : m_shaderModule(shaderModule) {}
public:
    Shader(const Shader& other) = delete;
    Shader(Shader&& other) noexcept = delete;

    Shader& operator =(const Shader& other) = delete;
    Shader& operator =(Shader&& other) noexcept = delete;

    [[nodiscard]] static std::unique_ptr<Shader> create(WGPUDevice device, const std::string& source) {
        WGPUShaderModuleWGSLDescriptor shaderModuleWgslDescriptor = {
                .chain = WGPUChainedStruct{
                        .sType = WGPUSType_ShaderModuleWGSLDescriptor,
                },
                .code = source.c_str(),
        };
        WGPUShaderModuleDescriptor shaderModuleDescriptor = {
                .nextInChain = reinterpret_cast<WGPUChainedStruct *>(&shaderModuleWgslDescriptor),
                .label = "Shader module",
        };
        WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(device, &shaderModuleDescriptor);

        return std::unique_ptr<Shader>(new Shader(shaderModule));
    }

    [[nodiscard]] static std::unique_ptr<Shader> createFromFile(WGPUDevice device, const std::string& path) {
        std::ifstream stream(path, std::ios_base::binary);
        std::string source = { std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>() };

        return Shader::create(device, source);
    }

    [[nodiscard]] WGPUShaderModule shaderModule() const {
        return m_shaderModule;
    }
};
