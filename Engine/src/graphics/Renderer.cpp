#include "delusion/graphics/Renderer.hpp"

#include <array>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

Renderer Renderer::create(WGPUDevice device, WGPUQueue queue, WGPUSurfaceCapabilities surfaceCapabilities) {
    auto shader = Shader::createFromFile(device, "src/shader.wgsl");

    static std::array<glm::vec2, 12> vertices = {
            glm::vec2(0.5f, 0.5f), glm::vec2(1.0f, 1.0f),
            glm::vec2(0.5f, -0.5f), glm::vec2(1.0f, 0.0f),
            glm::vec2(-0.5f, 0.5f), glm::vec2(0.0f, 1.0f),

            glm::vec2(0.5f, -0.5f), glm::vec2(1.0f, 0.0f),
            glm::vec2(-0.5f, -0.5f), glm::vec2(0.0f, 0.0f),
            glm::vec2(-0.5f, 0.5f), glm::vec2(0.0f, 1.0f),
    };

    WGPUBufferDescriptor vertexBufferDescriptor = {
            .nextInChain = nullptr,
            .label = "Vertex buffer",
            .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
            .size = 6 * (2 * sizeof(float) + 2 * sizeof(float)),
            .mappedAtCreation = false,
    };
    WGPUBuffer vertexBuffer = wgpuDeviceCreateBuffer(device, &vertexBufferDescriptor);

    wgpuQueueWriteBuffer(queue, vertexBuffer, 0, vertices.data(), 6 * (2 * sizeof(float) + 2 * sizeof(float)));

    return {device, queue, surfaceCapabilities, std::move(shader), vertexBuffer};
}

void Renderer::renderScene(WGPUCommandEncoder commandEncoder, WGPUTextureView renderTarget, Scene &scene) {
    std::array<WGPUVertexAttribute, 2> attributes = {
            WGPUVertexAttribute{
                    .format = WGPUVertexFormat_Float32x2,
                    .offset = 0,
                    .shaderLocation = 0,
            },
            WGPUVertexAttribute{
                    .format = WGPUVertexFormat_Float32x2,
                    .offset = sizeof(glm::vec2),
                    .shaderLocation = 1,
            },
    };
    WGPUVertexBufferLayout vertexBufferLayout = {
            .arrayStride = (2 * sizeof(float) + 2 * sizeof(float)),
            .stepMode = WGPUVertexStepMode_Vertex,
            .attributeCount = attributes.size(),
            .attributes = attributes.data(),
    };
    WGPUVertexState vertexState = {
            .module = shader->shaderModule(),
            .entryPoint = "vs_main",
            .bufferCount = 1,
            .buffers = &vertexBufferLayout,
    };

    WGPUBlendState blendState = {
            .color = WGPUBlendComponent{
                    .operation = WGPUBlendOperation_Add,
                    .srcFactor = WGPUBlendFactor_SrcAlpha,
                    .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha,
            },
            .alpha = WGPUBlendComponent{
                    .operation = WGPUBlendOperation_Add,
                    .srcFactor = WGPUBlendFactor_Zero,
                    .dstFactor = WGPUBlendFactor_One,
            }
    };
    WGPUColorTargetState targets[] = {
            WGPUColorTargetState{
                    .format = surfaceCapabilities.formats[0],
                    .blend = &blendState,
                    .writeMask = WGPUColorWriteMask_All,
            },
    };
    WGPUFragmentState fragmentState = {
            .module = shader->shaderModule(),
            .entryPoint = "fs_main",
            .targetCount = 1,
            .targets = targets,
    };

    WGPUSamplerDescriptor samplerDescriptor = {
            .addressModeU = WGPUAddressMode_ClampToEdge,
            .addressModeV = WGPUAddressMode_ClampToEdge,
            .addressModeW = WGPUAddressMode_ClampToEdge,
            .magFilter = WGPUFilterMode_Linear,
            .minFilter = WGPUFilterMode_Linear,
            .mipmapFilter = WGPUMipmapFilterMode_Linear,
            .lodMinClamp = 0.0f,
            .lodMaxClamp = 1.0f,
            .compare = WGPUCompareFunction_Undefined,
            .maxAnisotropy = 1,
    };
    WGPUSampler sampler = wgpuDeviceCreateSampler(device, &samplerDescriptor);

    for (Entity &entity: scene.entities()) {
        if (!entity.hasComponent<Transform>() || !entity.hasComponent<Sprite>())
            continue;

        auto &transform = entity.getComponent<Transform>();
        auto &sprite = entity.getComponent<Sprite>();

        if (sprite.texture == nullptr)
            continue;

        WGPUBufferDescriptor uniformBufferDescriptor = {
                .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
                .size = sizeof(glm::mat4),
                .mappedAtCreation = false,
        };
        WGPUBuffer uniformBuffer = wgpuDeviceCreateBuffer(device, &uniformBufferDescriptor);

        glm::mat4 transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(transform.position, 0.0f)) *
                                    glm::rotate(glm::mat4(1.0f), -transform.rotation, glm::vec3(0.0f, 0.0f, 1.0f)) *
                                    glm::scale(glm::mat4(1.0f), glm::vec3(transform.scale, 1.0f));

        wgpuQueueWriteBuffer(queue, uniformBuffer, 0, &transformMatrix, sizeof(transformMatrix));

        std::array<WGPUBindGroupLayoutEntry, 3> bindGroupLayoutEntries = {
                WGPUBindGroupLayoutEntry{
                        .nextInChain = nullptr,
                        .binding = 0,
                        .visibility = WGPUShaderStage_Fragment,
                        .texture = WGPUTextureBindingLayout{
                                .nextInChain = nullptr,
                                .sampleType = WGPUTextureSampleType_Float,
                                .viewDimension = WGPUTextureViewDimension_2D,
                                .multisampled = false,
                        },
                },
                WGPUBindGroupLayoutEntry{
                        .binding = 1,
                        .visibility = WGPUShaderStage_Fragment,
                        .sampler = WGPUSamplerBindingLayout{
                                .type = WGPUSamplerBindingType_Filtering,
                        },
                },
                WGPUBindGroupLayoutEntry{
                        .binding = 2,
                        .visibility = WGPUShaderStage_Vertex,
                        .buffer = WGPUBufferBindingLayout{
                                .type = WGPUBufferBindingType_Uniform,
                                .minBindingSize = sizeof(transformMatrix),
                        },
                },
        };
        WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {
                .nextInChain = nullptr,
                .entryCount = bindGroupLayoutEntries.size(),
                .entries = bindGroupLayoutEntries.data(),
        };
        WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(device, &bindGroupLayoutDescriptor);

        std::array<WGPUBindGroupEntry, 3> bindGroupEntries = {
                WGPUBindGroupEntry{
                        .nextInChain = nullptr,
                        .binding = 0,
                        .textureView = sprite.texture->view(),
                },
                WGPUBindGroupEntry{
                        .nextInChain = nullptr,
                        .binding = 1,
                        .sampler = sampler,
                },
                WGPUBindGroupEntry{
                        .nextInChain = nullptr,
                        .binding = 2,
                        .buffer = uniformBuffer,
                        .offset = 0,
                        .size = sizeof(transformMatrix),
                },
        };
        WGPUBindGroupDescriptor bindGroupDescriptor = {
                .nextInChain = nullptr,
                .layout = bindGroupLayout,
                .entryCount = bindGroupEntries.size(),
                .entries = bindGroupEntries.data(),
        };
        WGPUBindGroup bindGroup = wgpuDeviceCreateBindGroup(device, &bindGroupDescriptor);

        WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {
                .nextInChain = nullptr,
                .label = "Pipeline layout",
                .bindGroupLayoutCount = 1,
                .bindGroupLayouts = &bindGroupLayout,
        };
        WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(device, &pipelineLayoutDescriptor);

        WGPURenderPipelineDescriptor renderPipelineDescriptor = {
                .label = "Render pipeline",
                .layout = pipelineLayout,
                .vertex = vertexState,
                .primitive = WGPUPrimitiveState{
                        .topology = WGPUPrimitiveTopology_TriangleList,
                },
                .multisample = WGPUMultisampleState{
                        .count = 1,
                        .mask = 0xFFFFFFFF,
                },
                .fragment = &fragmentState,
        };
        WGPURenderPipeline renderPipeline = wgpuDeviceCreateRenderPipeline(device, &renderPipelineDescriptor);

        WGPURenderPassColorAttachment renderPassColorAttachment = {
                .view = renderTarget,
                .resolveTarget = nullptr,
                .loadOp = WGPULoadOp_Clear,
                .storeOp = WGPUStoreOp_Store,
                .clearValue = WGPUColor{0.0, 0.0, 0.0, 1.0},
        };
        WGPURenderPassDescriptor renderPassDescriptor = {
                .nextInChain = nullptr,
                .colorAttachmentCount = 1,
                .colorAttachments = &renderPassColorAttachment,
                .depthStencilAttachment = nullptr,
                .timestampWrites = nullptr,
        };
        WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder,
                                                                                    &renderPassDescriptor);

        wgpuRenderPassEncoderSetPipeline(renderPassEncoder, renderPipeline);
        wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0, bindGroup, 0, nullptr);
        wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0, quadVertexBuffer, 0,
                                             6 * (2 * sizeof(float) + 2 * sizeof(float)));

        wgpuRenderPassEncoderDraw(renderPassEncoder, 6, 1, 0, 0);

        wgpuRenderPassEncoderEnd(renderPassEncoder);

        wgpuRenderPipelineRelease(renderPipeline);
        wgpuPipelineLayoutReference(pipelineLayout);
        wgpuBindGroupRelease(bindGroup);
        wgpuBindGroupLayoutRelease(bindGroupLayout);
        wgpuBufferRelease(uniformBuffer);
    }
}
