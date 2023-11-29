module;

#include <filesystem>

#include <cimage.h>

export module image_decoder;

import image;

export class ImageDecoder {
public:
    static Image decode(const std::string& path) {
        cimage_image* image = cimage_image_decode_from_file(path.c_str());

        uint32_t width = image->width;
        uint32_t height = image->height;
        
        std::vector<uint8_t> pixels(width * height * 4);

        std::memcpy(pixels.data(), image->pixels, width * height * 4);

        cimage_image_free(image);
        
        return { width, height, pixels };
    }
};
