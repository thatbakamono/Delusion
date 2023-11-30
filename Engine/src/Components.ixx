module;

#include <memory>

#include <glm/vec2.hpp>

export module components;

import texture2d;

export struct Transform {
    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    glm::vec2 scale = glm::vec2(1.0f, 1.0f);
    float rotation = 0.0f;
};

export struct Sprite {
    std::shared_ptr<Texture2D> texture;
};
