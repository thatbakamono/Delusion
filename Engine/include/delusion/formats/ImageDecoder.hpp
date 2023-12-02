#pragma once

#include <string>

#include "delusion/Image.hpp"

class ImageDecoder {
public:
    static Image decode(const std::string& path);
};
