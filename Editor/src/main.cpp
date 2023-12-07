#include <cassert>
#include <iostream>
#include <fstream>
#include <memory>

#include <glfw3webgpu.h>
#include <webgpu.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_wgpu.h>
#include <nfd.hpp>

#include <delusion/Engine.hpp>
#include <delusion/Scene.hpp>
#include <delusion/Window.hpp>
#include <delusion/formats/ImageDecoder.hpp>
#include <delusion/graphics/Renderer.hpp>

#include "Editor.hpp"

int main() {
    Engine engine;

    NFD_Init();

    WGPUInstanceDescriptor descriptor = {
            .nextInChain = nullptr
    };
    WGPUInstance instance = wgpuCreateInstance(&descriptor);

    Window window("", 1280, 720);

    WGPUSurface surface = glfwGetWGPUSurface(instance, window.inner());

    WGPURequestAdapterOptions adapterOptions = {
            .nextInChain = nullptr,
            .compatibleSurface = surface,
    };

    WGPUAdapter adapter = requestAdapter(instance, &adapterOptions);

    WGPUDeviceDescriptor deviceDescriptor = {
            .nextInChain = nullptr,
            .label = "Device",
            .requiredFeatureCount = 0,
            .requiredLimits = nullptr,
            .defaultQueue = WGPUQueueDescriptor{
                    .nextInChain = nullptr,
                    .label = "Queue",
            },
    };

    WGPUDevice device = requestDevice(adapter, &deviceDescriptor);

    auto onDeviceError = [](WGPUErrorType type, char const *message, void *) {
        std::cout << "Uncaptured m_device error: type " << type;
        if (message) std::cout << " (" << message << ")";
        std::cout << std::endl;
    };
    wgpuDeviceSetUncapturedErrorCallback(device, onDeviceError, nullptr);

    WGPUQueue queue = wgpuDeviceGetQueue(device);

    auto onQueueWorkDone = [](WGPUQueueWorkDoneStatus status, void *) {
        std::cout << "Queued work finished with status: " << status << std::endl;
    };
    wgpuQueueOnSubmittedWorkDone(queue, onQueueWorkDone, nullptr);

    WGPUSurfaceConfiguration surfaceConfiguration = {
            .nextInChain = nullptr,
            .device = device,
            .usage = WGPUTextureUsage_RenderAttachment,
            .viewFormatCount = 0,
            .viewFormats = nullptr,
            .width = 1280,
            .height = 720,
            .presentMode = WGPUPresentMode_Fifo,
    };

    WGPUSurfaceCapabilities surfaceCapabilities = {};
    wgpuSurfaceGetCapabilities(surface, adapter, &surfaceCapabilities);

    assert(surfaceCapabilities.alphaModeCount > 0);
    assert(surfaceCapabilities.formatCount > 0);

    surfaceConfiguration.alphaMode = surfaceCapabilities.alphaModes[0];
    surfaceConfiguration.format = surfaceCapabilities.formats[0];

    wgpuSurfaceConfigure(surface, &surfaceConfiguration);

    WGPUTextureFormat preferredFormat = wgpuSurfaceGetPreferredFormat(surface, adapter);

    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    std::shared_ptr<Texture2D> viewportTexture = Texture2D::create(UniqueId(), device, 1280, 720, true);

    ImGui_ImplGlfw_InitForOther(window.inner(), true);
    ImGui_ImplWGPU_Init(device, 3, preferredFormat, WGPUTextureFormat_Undefined);

    Renderer renderer = Renderer::create(device, queue, surfaceCapabilities);

    auto fileIconImage = ImageDecoder::decode("file.png");
    auto directoryIconImage = ImageDecoder::decode("directory.png");
    std::shared_ptr<Texture2D> fileIconTexture = Texture2D::create(UniqueId(), device, queue, fileIconImage);
    std::shared_ptr<Texture2D> directoryIconTexture = Texture2D::create(UniqueId(), device, queue, directoryIconImage);

    Image emptyImage(1, 1, { 0, 0, 0, 0 });
    std::shared_ptr<Texture2D> emptyTexture = Texture2D::create(UniqueId(), device, queue, emptyImage);

    Editor editor(device, queue, emptyTexture, fileIconTexture, directoryIconTexture);

    while (window.isOpen()) {
        engine.pollEvents();

        WGPUCommandEncoderDescriptor encoderDescriptor = {
                .nextInChain = nullptr,
                .label = "Command encoder",
        };
        WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(device, &encoderDescriptor);

        {
            auto& scene = editor.scene();

            if (scene.has_value()) {
                renderer.renderScene(commandEncoder, viewportTexture->view(), scene.value());
            }
        }

        {
            ImGui_ImplWGPU_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

            editor.update(viewportTexture.get());

            ImGui::Render();
        }

        WGPUSurfaceTexture surfaceTexture;
        wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);

        if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_Success) {
            continue;
        }

        auto textureView = wgpuTextureCreateView(surfaceTexture.texture, nullptr);

        WGPURenderPassColorAttachment renderPassColorAttachment = {
                .view = textureView,
                .resolveTarget = nullptr,
                .loadOp = WGPULoadOp_Clear,
                .storeOp = WGPUStoreOp_Store,
                .clearValue = WGPUColor{0.3, 0.3, 0.3, 1.0},
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

        ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);

        wgpuRenderPassEncoderEnd(renderPassEncoder);

        WGPUCommandBufferDescriptor commandBufferDescriptor = {
                .nextInChain = nullptr,
                .label = "Command buffer",
        };
        WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, &commandBufferDescriptor);

        wgpuQueueSubmit(queue, 1, &commandBuffer);
        wgpuSurfacePresent(surface);

        wgpuCommandBufferReference(commandBuffer);
        wgpuRenderPassEncoderRelease(renderPassEncoder);
        wgpuCommandEncoderRelease(commandEncoder);
        wgpuTextureViewRelease(textureView);
        wgpuTextureRelease(surfaceTexture.texture);
    }

    wgpuQueueRelease(queue);
    wgpuDeviceRelease(device);
    wgpuAdapterRelease(adapter);
    wgpuSurfaceRelease(surface);
    wgpuInstanceRelease(instance);

    NFD_Quit();

    return 0;
}
