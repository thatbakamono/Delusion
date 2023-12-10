#pragma once

#include <box2d/box2d.h>

#include "delusion/Entity.hpp"

class Scene {
    private:
        entt::registry m_registry;

        std::vector<Entity> m_entities;

        std::unique_ptr<b2World> m_physicsWorld;
    public:
        Scene() = default;

        Scene(const Scene &other) = delete;

        Scene(Scene &&other) noexcept;

        Scene &operator=(const Scene &other) = delete;

        Scene &operator=(Scene &&other) noexcept;

        void start();

        void stop();

        void onUpdate(float deltaTime);

        Entity &create();

        void remove(Entity &entity);

        [[nodiscard]] std::vector<Entity> &entities() {
            return m_entities;
        }

        [[nodiscard]] const std::vector<Entity> &entities() const {
            return m_entities;
        }
    private:
        void updateRegistry(Entity &entity, entt::registry *registry);
};
