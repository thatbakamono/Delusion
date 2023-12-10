#pragma once

#include <cassert>
#include <iostream>

#include <glfw3webgpu.h>
#include <webgpu.h>

#include "delusion/Window.hpp"

class GraphicsBackend {
    private:
        WGPUInstance m_instance {};

        WGPUAdapter m_adapter {};
        WGPUDevice m_device {};
        WGPUQueue m_queue {};

        WGPUSurface m_surface {};
        WGPUSurfaceCapabilities m_surfaceCapabilities = {};

        WGPUTextureFormat m_preferredFormat {};
    public:
        GraphicsBackend();

        ~GraphicsBackend();

        void setup(const Window &window);

        void configureSurface(uint32_t width, uint32_t height);

        [[nodiscard]] WGPUDevice device() const {
            return m_device;
        }

        [[nodiscard]] WGPUQueue queue() const {
            return m_queue;
        }

        [[nodiscard]] WGPUSurface surface() const {
            return m_surface;
        }

        [[nodiscard]] const WGPUSurfaceCapabilities &surfaceCapabilities() const {
            return m_surfaceCapabilities;
        }

        [[nodiscard]] WGPUTextureFormat preferredFormat() const {
            return m_preferredFormat;
        }
    private:
        [[nodiscard]] WGPUAdapter requestAdapter(WGPUInstance instance, WGPURequestAdapterOptions const *options);

        [[nodiscard]] WGPUDevice requestDevice(WGPUAdapter adapter, WGPUDeviceDescriptor const *descriptor);
};
