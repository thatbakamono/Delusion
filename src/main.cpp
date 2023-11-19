#include <array>
#include <cassert>
#include <iostream>
#include <fstream>
#include <memory>

#include <glfw/glfw3.h>
#include <webgpu.h>
#include <glfw3webgpu.h>

WGPUAdapter requestAdapter(WGPUInstance instance, WGPURequestAdapterOptions const *options) {
    struct UserData {
        WGPUAdapter adapter = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    static auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const *message,
                                           void *pUserData) {
        UserData &userData = *reinterpret_cast<UserData *>(pUserData);
        if (status == WGPURequestAdapterStatus_Success) {
            userData.adapter = adapter;
        } else {
            std::cout << "Could not get WebGPU adapter: " << message << std::endl;
        }
        userData.requestEnded = true;
    };

    wgpuInstanceRequestAdapter(
            instance,
            options,
            onAdapterRequestEnded,
            &userData
    );

    assert(userData.requestEnded);

    return userData.adapter;
}

WGPUDevice requestDevice(WGPUAdapter adapter, WGPUDeviceDescriptor const *descriptor) {
    struct UserData {
        WGPUDevice device = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    static auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, char const *message,
                                          void *pUserData) {
        UserData &userData = *reinterpret_cast<UserData *>(pUserData);
        if (status == WGPURequestDeviceStatus_Success) {
            userData.device = device;
        } else {
            std::cout << "Could not get WebGPU device: " << message << std::endl;
        }
        userData.requestEnded = true;
    };

    wgpuAdapterRequestDevice(
            adapter,
            descriptor,
            onDeviceRequestEnded,
            &userData
    );

    assert(userData.requestEnded);

    return userData.device;
}

int main() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    WGPUInstanceDescriptor descriptor = {
            .nextInChain = nullptr
    };
    WGPUInstance instance = wgpuCreateInstance(&descriptor);

    std::unique_ptr<GLFWwindow, decltype(glfwDestroyWindow) *> window{
            glfwCreateWindow(1280, 720, "", nullptr, nullptr),
            &glfwDestroyWindow
    };

    WGPUSurface surface = glfwGetWGPUSurface(instance, window.get());

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
        std::cout << "Uncaptured device error: type " << type;
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

    WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {
            .label = "Pipeline layout",
    };
    WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(device, &pipelineLayoutDescriptor);

    std::ifstream stream("src/shader.wgsl", std::ios_base::binary);
    std::string shaderSource = {std::istreambuf_iterator(stream), std::istreambuf_iterator<char>()};

    WGPUShaderModuleWGSLDescriptor shaderModuleWgslDescriptor = {
            .chain = WGPUChainedStruct{
                    .sType = WGPUSType_ShaderModuleWGSLDescriptor,
            },
            .code = shaderSource.c_str(),
    };
    WGPUShaderModuleDescriptor shaderModuleDescriptor = {
            .nextInChain = reinterpret_cast<WGPUChainedStruct *>(&shaderModuleWgslDescriptor),
            .label = "Shader module",
    };
    WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(device, &shaderModuleDescriptor);

    std::array<float, 6> vertices {
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.0f,  0.5f,
    };

    WGPUBufferDescriptor vertexBufferDescriptor = {
            .nextInChain = nullptr,
            .label = "Vertex buffer",
            .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
            .size = vertices.size() * sizeof(float),
            .mappedAtCreation = false,
    };
    WGPUBuffer vertexBuffer = wgpuDeviceCreateBuffer(device, &vertexBufferDescriptor);
    wgpuQueueWriteBuffer(queue, vertexBuffer, 0, vertices.data(), vertices.size() * sizeof(float));

    WGPUVertexAttribute attributes[] = {
        WGPUVertexAttribute {
            .format = WGPUVertexFormat_Float32x2,
            .offset = 0,
            .shaderLocation = 0,
        }
    };
    WGPUVertexBufferLayout vertexBufferLayout = {
            .arrayStride = 2 * sizeof(float),
            .stepMode = WGPUVertexStepMode_Vertex,
            .attributeCount = 1,
            .attributes = attributes,
    };

    WGPUColorTargetState targets[] = {
            WGPUColorTargetState{
                    .format = surfaceCapabilities.formats[0],
                    .writeMask = WGPUColorWriteMask_All,
            },
    };
    WGPUVertexState vertexState = {
            .module = shaderModule,
            .entryPoint = "vs_main",
            .bufferCount = 1,
            .buffers = &vertexBufferLayout,
    };
    WGPUFragmentState fragmentState = {
            .module = shaderModule,
            .entryPoint = "fs_main",
            .targetCount = 1,
            .targets = targets,
    };
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

    while (!glfwWindowShouldClose(window.get())) {
        glfwPollEvents();

        WGPUSurfaceTexture surfaceTexture;
        wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);

        auto textureView = wgpuTextureCreateView(surfaceTexture.texture, nullptr);

        WGPUCommandEncoderDescriptor encoderDescriptor = {
                .nextInChain = nullptr,
                .label = "Command encoder",
        };
        WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(device, &encoderDescriptor);

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

        wgpuRenderPassEncoderSetPipeline(renderPassEncoder, renderPipeline);
        wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0, vertexBuffer, 0, vertices.size() * sizeof(float));
        wgpuRenderPassEncoderDraw(renderPassEncoder, 3, 1, 0, 0);
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

    wgpuBufferRelease(vertexBuffer);
    wgpuQueueRelease(queue);
    wgpuDeviceRelease(device);
    wgpuAdapterRelease(adapter);
    wgpuSurfaceRelease(surface);
    wgpuInstanceRelease(instance);

    glfwTerminate();

    return 0;
}
