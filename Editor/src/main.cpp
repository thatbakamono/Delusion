#include <memory>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_wgpu.h>
#include <nfd.hpp>
#include <webgpu.h>

#include <delusion/Engine.hpp>
#include <delusion/formats/ImageDecoder.hpp>
#include <delusion/graphics/GraphicsBackend.hpp>
#include <delusion/graphics/Renderer.hpp>
#include <delusion/scripting/ScriptEngine.hpp>
#include <delusion/Window.hpp>

#include "Editor.hpp"

int main() {
    constexpr int defaultWindowWidth = 1280;
    constexpr int defaultWindowHeight = 720;

    Engine *engine = Engine::get();

    NFD_Init();

    GraphicsBackend backend;

    auto window = std::make_shared<Window>("", defaultWindowWidth, defaultWindowHeight);

    engine->setCurrentWindow(window);

    backend.setup(window.get());
    backend.configureSurface(defaultWindowWidth, defaultWindowHeight);

    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    std::shared_ptr<Texture2D> viewportTexture =
        Texture2D::create(UniqueId(), backend.device(), defaultWindowWidth, defaultWindowHeight, true);

    ImGui_ImplGlfw_InitForOther(window->inner(), true);
    ImGui_ImplWGPU_Init(backend.device(), 3, backend.preferredFormat(), WGPUTextureFormat_Undefined);

    Renderer renderer = Renderer::create(backend.device(), backend.queue(), backend.surfaceCapabilities());

    auto scriptEngine = std::make_shared<ScriptEngine>();

    auto fileIconImage = ImageDecoder::decode("file.png");
    auto directoryIconImage = ImageDecoder::decode("directory.png");
    auto playIconImage = ImageDecoder::decode("play.png");
    auto stopIconImage = ImageDecoder::decode("stop.png");
    std::shared_ptr<Texture2D> fileIconTexture =
        Texture2D::create(UniqueId(), backend.device(), backend.queue(), fileIconImage);
    std::shared_ptr<Texture2D> directoryIconTexture =
        Texture2D::create(UniqueId(), backend.device(), backend.queue(), directoryIconImage);
    std::shared_ptr<Texture2D> playIconTexture =
        Texture2D::create(UniqueId(), backend.device(), backend.queue(), playIconImage);
    std::shared_ptr<Texture2D> stopIconTexture =
        Texture2D::create(UniqueId(), backend.device(), backend.queue(), stopIconImage);

    Image emptyImage(1, 1, { 0, 0, 0, 0 });
    std::shared_ptr<Texture2D> emptyTexture =
        Texture2D::create(UniqueId(), backend.device(), backend.queue(), emptyImage);

    Editor editor(
        engine, backend.device(), backend.queue(), scriptEngine, emptyTexture, fileIconTexture, directoryIconTexture,
        playIconTexture, stopIconTexture
    );

    int previousWidth = defaultWindowWidth;
    int previousHeight = defaultWindowHeight;

    float lastFrameTime {};
    float currentFrameTime {};
    float deltaTime {};

    while (window->isOpen()) {
        currentFrameTime = static_cast<float>(glfwGetTime());
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        engine->pollEvents();

        int currentWidth {};
        int currentHeight {};

        glfwGetWindowSize(window->inner(), &currentWidth, &currentHeight);

        editor.onRuntimeUpdate(deltaTime);

        WGPUSurfaceTexture surfaceTexture;
        wgpuSurfaceGetCurrentTexture(backend.surface(), &surfaceTexture);

        if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_Success) {
            continue;
        }

        if (currentWidth != previousWidth || currentHeight != previousHeight) {
            if (currentWidth > 0 && currentHeight > 0) {
                backend.configureSurface(static_cast<uint32_t>(currentWidth), static_cast<uint32_t>(currentHeight));
            }

            previousWidth = currentWidth;
            previousHeight = currentHeight;
        }

        WGPUCommandEncoderDescriptor encoderDescriptor = {
            .nextInChain = nullptr,
            .label = "Command encoder",
        };
        WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(backend.device(), &encoderDescriptor);

        {
            auto *scene = engine->currentScene();

            if (scene != nullptr) {
                renderer.renderScene(commandEncoder, viewportTexture->view(), editor.camera(), *scene);
            }
        }

        {
            ImGui_ImplWGPU_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

            editor.onEditorUpdate(viewportTexture, deltaTime);

            ImGui::Render();
        }

        auto textureView = wgpuTextureCreateView(surfaceTexture.texture, nullptr);

        WGPURenderPassColorAttachment renderPassColorAttachment = {
            .view = textureView,
            .resolveTarget = nullptr,
            .loadOp = WGPULoadOp_Clear,
            .storeOp = WGPUStoreOp_Store,
            .clearValue = WGPUColor { 0.3, 0.3, 0.3, 1.0 },
        };
        WGPURenderPassDescriptor renderPassDescriptor = {
            .nextInChain = nullptr,
            .colorAttachmentCount = 1,
            .colorAttachments = &renderPassColorAttachment,
            .depthStencilAttachment = nullptr,
            .timestampWrites = nullptr,
        };
        WGPURenderPassEncoder renderPassEncoder =
            wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);

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
