#pragma once

#include <cassert>
#include <iostream>

#include <glfw/glfw3.h>
#include <webgpu.h>

class Engine {
public:
    Engine() {
        if (glfwInit() != GLFW_TRUE) {
            throw std::exception("GLFW initialization failed");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    ~Engine() {
        glfwTerminate();
    }

    void pollEvents() {
        glfwPollEvents();
    }
};

[[nodiscard]] WGPUAdapter requestAdapter(WGPUInstance instance, WGPURequestAdapterOptions const *options);

[[nodiscard]] WGPUDevice requestDevice(WGPUAdapter adapter, WGPUDeviceDescriptor const *descriptor);
