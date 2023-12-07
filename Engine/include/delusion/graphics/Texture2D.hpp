#pragma once

#include <memory>

#include <webgpu.h>

#include "delusion/Image.hpp"
#include "delusion/UniqueId.hpp"

class Texture2D {
private:
    UniqueId m_id;

    WGPUTexture m_texture;
    WGPUTextureView  m_textureView;

    Texture2D(UniqueId id, WGPUTexture texture, WGPUTextureView textureView) : m_id(id), m_texture(texture), m_textureView(textureView) {}
public:
    Texture2D(const Texture2D& other) = delete;
    Texture2D(Texture2D&& other) noexcept = delete;

    ~Texture2D();

    Texture2D& operator =(const Texture2D& other) = delete;
    Texture2D& operator =(Texture2D&& other) noexcept = delete;

    [[nodiscard]] static std::unique_ptr<Texture2D> create(UniqueId id, WGPUDevice device, WGPUQueue queue, Image& image);

    [[nodiscard]] UniqueId id() {
        return m_id;
    }

    [[nodiscard]] WGPUTextureView view() {
        return m_textureView;
    }
};
