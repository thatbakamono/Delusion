#pragma once

#include <memory>

#include <glm/vec2.hpp>

#include "delusion/graphics/Texture2D.hpp"

class Scene;

struct TransformComponent {
        glm::vec2 position = glm::vec2(0.0f, 0.0f);
        glm::vec2 scale = glm::vec2(1.0f, 1.0f);
        float rotation = 0.0f;

        TransformComponent() = default;

        TransformComponent(glm::vec2 position, glm::vec2 scale, float rotation)
            : position(position), scale(scale), rotation(rotation) {}
};

struct SpriteComponent {
        std::shared_ptr<Texture2D> texture;
};

struct RigidbodyComponent {
        enum class BodyType : int {
            Static = 0,
            Dynamic = 1,
            Kinematic = 2
        };

        BodyType bodyType = BodyType::Static;
        bool hasFixedRotation = false;

        float density = 1.0f;
        float friction = 0.5f;
        float restitution = 0.0f;
        float restitutionThreshold = 0.5f;

        RigidbodyComponent() = default;

        RigidbodyComponent(RigidbodyComponent &other) {
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

struct BoxColliderComponent {
        glm::vec2 size { 1.0f, 1.0f };
        glm::vec2 offset;
};

struct ScriptComponent {
        std::string name;

        void *class_ {};
        void *instance {};
};
