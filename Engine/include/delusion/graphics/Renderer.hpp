#pragma once

#include <webgpu.h>

#include "delusion/Components.hpp"
#include "delusion/Scene.hpp"
#include "delusion/graphics/OrthographicCamera.hpp"
#include "delusion/graphics/Shader.hpp"

class Renderer {
private:
    WGPUDevice device;
    WGPUQueue queue;
    WGPUSurfaceCapabilities surfaceCapabilities;

    std::unique_ptr<Shader> shader;

    WGPUBuffer quadVertexBuffer;

    Renderer(
            WGPUDevice device,
            WGPUQueue queue,
            WGPUSurfaceCapabilities surfaceCapabilities,
            std::unique_ptr<Shader> shader,
            WGPUBuffer quadVertexBuffer
    ) : device(device), queue(queue), surfaceCapabilities(surfaceCapabilities), shader(std::move(shader)),
        quadVertexBuffer(quadVertexBuffer) {}

public:
    Renderer(const Renderer &other) = delete;

    Renderer(Renderer &&other) noexcept = delete;

    Renderer &operator=(const Renderer &other) = delete;

    Renderer &operator=(Renderer &&other) noexcept = delete;

    [[nodiscard]] static Renderer create(WGPUDevice device, WGPUQueue queue, WGPUSurfaceCapabilities surfaceCapabilities);

    void renderScene(WGPUCommandEncoder commandEncoder, WGPUTextureView renderTarget, OrthographicCamera& camera, Scene &scene);
};
