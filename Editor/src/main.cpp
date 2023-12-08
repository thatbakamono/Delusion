#include <memory>

#include <webgpu.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_wgpu.h>
#include <nfd.hpp>

#include <delusion/Engine.hpp>
#include <delusion/Window.hpp>
#include <delusion/formats/ImageDecoder.hpp>
#include <delusion/graphics/GraphicsBackend.hpp>
#include <delusion/graphics/Renderer.hpp>

#include "Editor.hpp"

int main() {
    constexpr int defaultWindowWidth = 1280;
    constexpr int defaultWindowHeight = 720;

    Engine engine;

    NFD_Init();

    GraphicsBackend backend;

    Window window("", defaultWindowWidth, defaultWindowHeight);

    backend.setup(window);
    backend.configureSurface(defaultWindowWidth, defaultWindowHeight);

    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    std::shared_ptr<Texture2D> viewportTexture = Texture2D::create(UniqueId(), backend.device(), defaultWindowWidth, defaultWindowHeight, true);

    ImGui_ImplGlfw_InitForOther(window.inner(), true);
    ImGui_ImplWGPU_Init(backend.device(), 3, backend.preferredFormat(), WGPUTextureFormat_Undefined);

    Renderer renderer = Renderer::create(backend.device(), backend.queue(), backend.surfaceCapabilities());

    auto fileIconImage = ImageDecoder::decode("file.png");
    auto directoryIconImage = ImageDecoder::decode("directory.png");
    auto playIconImage = ImageDecoder::decode("play.png");
    auto stopIconImage = ImageDecoder::decode("stop.png");
    std::shared_ptr<Texture2D> fileIconTexture = Texture2D::create(UniqueId(), backend.device(), backend.queue(), fileIconImage);
    std::shared_ptr<Texture2D> directoryIconTexture = Texture2D::create(UniqueId(), backend.device(), backend.queue(), directoryIconImage);
    std::shared_ptr<Texture2D> playIconTexture = Texture2D::create(UniqueId(), backend.device(), backend.queue(), playIconImage);
    std::shared_ptr<Texture2D> stopIconTexture = Texture2D::create(UniqueId(), backend.device(), backend.queue(), stopIconImage);

    Image emptyImage(1, 1, { 0, 0, 0, 0 });
    std::shared_ptr<Texture2D> emptyTexture = Texture2D::create(UniqueId(), backend.device(), backend.queue(), emptyImage);

    Editor editor(backend.device(), backend.queue(), emptyTexture, fileIconTexture, directoryIconTexture, playIconTexture, stopIconTexture);

    int previousWidth = defaultWindowWidth;
    int previousHeight = defaultWindowHeight;

    float lastFrameTime {};
    float currentFrameTime {};
    float deltaTime {};

    while (window.isOpen()) {
        currentFrameTime = static_cast<float>(glfwGetTime());
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        engine.pollEvents();

        int currentWidth {};
        int currentHeight {};

        glfwGetWindowSize(window.inner(), &currentWidth, &currentHeight);

        if (currentWidth != previousWidth || currentHeight != previousHeight) {
            backend.configureSurface(static_cast<uint32_t>(currentWidth), static_cast<uint32_t>(currentHeight));

            previousWidth = currentWidth;
            previousHeight = currentHeight;
        }

        WGPUCommandEncoderDescriptor encoderDescriptor = {
                .nextInChain = nullptr,
                .label = "Command encoder",
        };
        WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(backend.device(), &encoderDescriptor);

        {
            auto& scene = editor.scene();

            if (scene.has_value()) {
                renderer.renderScene(commandEncoder, viewportTexture->view(), editor.camera(), scene.value());
            }
        }

        {
            ImGui_ImplWGPU_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

            editor.update(viewportTexture, deltaTime);

            ImGui::Render();
        }

        WGPUSurfaceTexture surfaceTexture;
        wgpuSurfaceGetCurrentTexture(backend.surface(), &surfaceTexture);

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

        wgpuQueueSubmit(backend.queue(), 1, &commandBuffer);
        wgpuSurfacePresent(backend.surface());

        wgpuCommandBufferReference(commandBuffer);
        wgpuRenderPassEncoderRelease(renderPassEncoder);
        wgpuCommandEncoderRelease(commandEncoder);
        wgpuTextureViewRelease(textureView);
        wgpuTextureRelease(surfaceTexture.texture);
    }

    ImGui_ImplWGPU_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    
    ImGui::DestroyContext();

    NFD_Quit();

    return 0;
}
