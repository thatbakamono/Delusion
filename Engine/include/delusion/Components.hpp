#pragma once

#include <memory>

#include <glm/vec2.hpp>

#include "delusion/graphics/Texture2D.hpp"

struct Transform {
    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    glm::vec2 scale = glm::vec2(1.0f, 1.0f);
    float rotation = 0.0f;
};

struct Sprite {
    std::shared_ptr<Texture2D> texture;
};
