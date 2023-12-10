#include "delusion/graphics/Texture2D.hpp"

Texture2D::~Texture2D() {
    wgpuTextureViewRelease(m_textureView);
    wgpuTextureRelease(m_texture);
}

std::unique_ptr<Texture2D> Texture2D::create(UniqueId id, WGPUDevice device, WGPUQueue queue, Image &image) {
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

    wgpuQueueWriteTexture(
        queue, &destination, image.pixels().data(), image.pixels().size(), &source, &textureDescriptor.size
    );

    return std::unique_ptr<Texture2D>(new Texture2D(id, texture, textureView, image.width(), image.height()));
}

std::unique_ptr<Texture2D> Texture2D::create(
    UniqueId id, WGPUDevice device, uint32_t width, uint32_t height, bool isRenderAttachment
) {
    int usage = WGPUTextureUsage_TextureBinding;

    if (isRenderAttachment) {
        usage |= WGPUTextureUsage_RenderAttachment;
    }

    WGPUTextureDescriptor textureDescriptor = {
        .nextInChain = nullptr,
        .label = "Texture2D",
        .usage = static_cast<WGPUTextureUsageFlags>(usage),
        .dimension = WGPUTextureDimension_2D,
        .size = WGPUExtent3D { width, height, 1 },
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

    return std::unique_ptr<Texture2D>(new Texture2D(id, texture, textureView, width, height));
}
