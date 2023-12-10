#include "delusion/graphics/GraphicsBackend.hpp"

GraphicsBackend::GraphicsBackend() {
    WGPUInstanceDescriptor descriptor = { .nextInChain = nullptr };

    m_instance = wgpuCreateInstance(&descriptor);
}

GraphicsBackend::~GraphicsBackend() {
    if (m_queue != nullptr) {
        wgpuQueueRelease(m_queue);
    }

    if (m_device != nullptr) {
        wgpuDeviceRelease(m_device);
    }

    if (m_adapter != nullptr) {
        wgpuAdapterRelease(m_adapter);
    }

    if (m_surface != nullptr) {
        wgpuSurfaceRelease(m_surface);
    }

    if (m_instance != nullptr) {
        wgpuInstanceRelease(m_instance);
    }
}

void GraphicsBackend::setup(const Window &window) {
    m_surface = glfwGetWGPUSurface(m_instance, window.inner());

    WGPURequestAdapterOptions adapterOptions = {
        .nextInChain = nullptr,
        .compatibleSurface = m_surface,
    };

    m_adapter = requestAdapter(m_instance, &adapterOptions);

    WGPUDeviceDescriptor deviceDescriptor = {
        .nextInChain = nullptr,
        .label = "Device",
        .requiredFeatureCount = 0,
        .requiredLimits = nullptr,
        .defaultQueue =
            WGPUQueueDescriptor {
                .nextInChain = nullptr,
                .label = "Queue",
            },
    };

    m_device = requestDevice(m_adapter, &deviceDescriptor);

    auto onDeviceError = [](WGPUErrorType type, char const *message, void *) {
        std::cout << "Uncaptured m_device error: type " << type;
        if (message)
            std::cout << " (" << message << ")";
        std::cout << std::endl;
    };
    wgpuDeviceSetUncapturedErrorCallback(m_device, onDeviceError, nullptr);

    m_queue = wgpuDeviceGetQueue(m_device);

    auto onQueueWorkDone = [](WGPUQueueWorkDoneStatus status, void *) {
        std::cout << "Queued work finished with status: " << status << std::endl;
    };
    wgpuQueueOnSubmittedWorkDone(m_queue, onQueueWorkDone, nullptr);

    wgpuSurfaceGetCapabilities(m_surface, m_adapter, &m_surfaceCapabilities);

    assert(m_surfaceCapabilities.alphaModeCount > 0);
    assert(m_surfaceCapabilities.formatCount > 0);

    m_preferredFormat = wgpuSurfaceGetPreferredFormat(m_surface, m_adapter);
}

void GraphicsBackend::configureSurface(uint32_t width, uint32_t height) {
    WGPUSurfaceConfiguration surfaceConfiguration = {
        .nextInChain = nullptr,
        .device = m_device,
        .format = m_surfaceCapabilities.formats[0],
        .usage = WGPUTextureUsage_RenderAttachment,
        .viewFormatCount = 0,
        .viewFormats = nullptr,
        .alphaMode = m_surfaceCapabilities.alphaModes[0],
        .width = width,
        .height = height,
        .presentMode = WGPUPresentMode_Fifo,
    };

    wgpuSurfaceConfigure(m_surface, &surfaceConfiguration);
}

WGPUAdapter GraphicsBackend::requestAdapter(WGPUInstance instance, const WGPURequestAdapterOptions *options) {
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
            std::cout << "Could not get WebGPU m_adapter: " << message << std::endl;
        }
        userData.requestEnded = true;
    };

    wgpuInstanceRequestAdapter(instance, options, onAdapterRequestEnded, &userData);

    assert(userData.requestEnded);

    return userData.adapter;
}

WGPUDevice GraphicsBackend::requestDevice(WGPUAdapter adapter, const WGPUDeviceDescriptor *descriptor) {
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
            std::cout << "Could not get WebGPU m_device: " << message << std::endl;
        }
        userData.requestEnded = true;
    };

    wgpuAdapterRequestDevice(adapter, descriptor, onDeviceRequestEnded, &userData);

    assert(userData.requestEnded);

    return userData.device;
}
