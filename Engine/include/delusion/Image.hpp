#pragma once

#include <span>
#include <vector>

class Image {
    private:
        uint32_t m_width;
        uint32_t m_height;
        std::vector<uint8_t> m_pixels;
    public:
        Image(uint32_t width, uint32_t height, std::vector<uint8_t> pixels)
            : m_width(width), m_height(height), m_pixels(std::move(pixels)) {}

        [[nodiscard]] uint32_t width() const {
            return m_width;
        }

        [[nodiscard]] uint32_t height() const {
            return m_height;
        }

        [[nodiscard]] std::span<uint8_t> pixels() {
            return { m_pixels };
        }
};
