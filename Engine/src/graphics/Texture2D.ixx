module;

#include <vector>
#include <memory>

#include <webgpu.h>

export module texture2d;

import image;

export class Texture2D {
private:
    WGPUTexture m_texture;
    WGPUTextureView  m_textureView;

    Texture2D(WGPUTexture texture, WGPUTextureView textureView) : m_texture(texture), m_textureView(textureView) {}
public:
    Texture2D(const Texture2D& other) = delete;
    Texture2D(Texture2D&& other) noexcept = delete;

    ~Texture2D() {
        wgpuTextureViewRelease(m_textureView);

        wgpuTextureDestroy(m_texture);
        wgpuTextureRelease(m_texture);
    }

    Texture2D& operator =(const Texture2D& other) = delete;
    Texture2D& operator =(Texture2D&& other) noexcept = delete;

    [[nodiscard]] static std::unique_ptr<Texture2D> create(WGPUDevice device, WGPUQueue queue, Image& image) {
        WGPUTextureDescriptor textureDescriptor = {
                .nextInChain = nullptr,
                .label = "Texture2D",
                .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
                .dimension = WGPUTextureDimension_2D,
                .size = WGPUExtent3D { image.width(), image.height(), 1 },
                .format = WGPUTextureFormat_RGBA8Unorm,
                .mipLevelCount = 1,
                .sampleCount = 1,
                .viewFormatCount = 0,
                .viewFormats = nullptr,
        };
        WGPUTexture texture = wgpuDeviceCreateTexture(device, &textureDescriptor);

        WGPUTextureViewDescriptor textureViewDescriptor = {
                .nextInChain = nullptr,
                .format = textureDescriptor.format,
                .dimension = WGPUTextureViewDimension_2D,
                .baseMipLevel = 0,
                .mipLevelCount = 1,
                .baseArrayLayer = 0,
                .arrayLayerCount = 1,
                .aspect = WGPUTextureAspect_All,
        };
        WGPUTextureView textureView = wgpuTextureCreateView(texture, &textureViewDescriptor);

        WGPUImageCopyTexture destination = {
                .nextInChain = nullptr,
                .texture = texture,
                .mipLevel = 0,
                .origin = { 0, 0, 0 },
                .aspect = WGPUTextureAspect_All,
        };

        WGPUTextureDataLayout source = {
                .nextInChain = nullptr,
                .offset = 0,
                .bytesPerRow = 4 * textureDescriptor.size.width,
                .rowsPerImage = textureDescriptor.size.height,
        };

        wgpuQueueWriteTexture(queue, &destination, image.pixels().data(), image.pixels().size(), &source, &textureDescriptor.size);

        return std::unique_ptr<Texture2D>(new Texture2D(texture, textureView));
    }

    [[nodiscard]] WGPUTextureView view() {
        return m_textureView;
    }
};
