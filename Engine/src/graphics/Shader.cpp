#include "delusion/graphics/Shader.hpp"

#include <fstream>

std::unique_ptr<Shader> Shader::create(WGPUDevice device, const std::string &source) {
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

std::unique_ptr<Shader> Shader::createFromFile(WGPUDevice device, const std::string &path) {
    std::ifstream stream(path, std::ios_base::binary);
    std::string source = { std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>() };

    return Shader::create(device, source);
}
