module;

#include <cassert>
#include <iostream>

#include <webgpu.h>

export module engine;

export WGPUAdapter requestAdapter(WGPUInstance instance, WGPURequestAdapterOptions const *options) {
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

    wgpuInstanceRequestAdapter(
            instance,
            options,
            onAdapterRequestEnded,
            &userData
    );

    assert(userData.requestEnded);

    return userData.adapter;
}

export WGPUDevice requestDevice(WGPUAdapter adapter, WGPUDeviceDescriptor const *descriptor) {
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

    wgpuAdapterRequestDevice(
            adapter,
            descriptor,
            onDeviceRequestEnded,
            &userData
    );

    assert(userData.requestEnded);

    return userData.device;
}
