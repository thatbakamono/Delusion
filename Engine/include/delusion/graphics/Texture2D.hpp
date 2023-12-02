#pragma once

#include <memory>

#include <webgpu.h>

#include "delusion/Image.hpp"

class Texture2D {
private:
    WGPUTexture m_texture;
    WGPUTextureView  m_textureView;

    Texture2D(WGPUTexture texture, WGPUTextureView textureView) : m_texture(texture), m_textureView(textureView) {}
public:
    Texture2D(const Texture2D& other) = delete;
    Texture2D(Texture2D&& other) noexcept = delete;

    ~Texture2D();

    Texture2D& operator =(const Texture2D& other) = delete;
    Texture2D& operator =(Texture2D&& other) noexcept = delete;

    [[nodiscard]] static std::unique_ptr<Texture2D> create(WGPUDevice device, WGPUQueue queue, Image& image);

    [[nodiscard]] WGPUTextureView view() {
        return m_textureView;
    }
};
