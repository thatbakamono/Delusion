#pragma once

#include <memory>

#include <glm/vec2.hpp>

#include "delusion/graphics/Texture2D.hpp"

class Scene;

struct Transform {
        glm::vec2 position = glm::vec2(0.0f, 0.0f);
        glm::vec2 scale = glm::vec2(1.0f, 1.0f);
        float rotation = 0.0f;

        Transform() = default;

        Transform(glm::vec2 position, glm::vec2 scale, float rotation)
            : position(position), scale(scale), rotation(rotation) {}
};

struct Sprite {
        std::shared_ptr<Texture2D> texture;
};

struct Rigidbody {
        enum class BodyType {
            Static,
            Dynamic,
            Kinematic
        };

        BodyType bodyType = BodyType::Static;
        bool hasFixedRotation = false;

        float density = 1.0f;
        float friction = 0.5f;
        float restitution = 0.0f;
        float restitutionThreshold = 0.5f;

        Rigidbody() = default;

        Rigidbody(Rigidbody &other) {
            bodyType = other.bodyType;
            hasFixedRotation = other.hasFixedRotation;
            density = other.density;
            friction = other.friction;
            restitution = other.restitution;
            restitutionThreshold = other.restitutionThreshold;
        }
    private:
        void *body {};
        void *fixture {};

        friend Scene;
};

struct BoxCollider {
        glm::vec2 size { 1.0f, 1.0f };
        glm::vec2 offset;
};
