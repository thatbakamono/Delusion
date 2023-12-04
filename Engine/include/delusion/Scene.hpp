#pragma once

#include <delusion/Entity.hpp>

class Scene {
private:
    entt::registry m_registry;

    std::vector<Entity> m_entities;
public:
    Scene() = default;

    Scene(const Scene& other) = delete;

    Scene(Scene&& other) noexcept;

    Scene& operator =(const Scene& other) = delete;

    Scene& operator =(Scene&& other) noexcept;

    Entity& create() {
        auto entityId = m_registry.create();

        m_entities.emplace_back(&m_registry, entityId);

        return m_entities.back();
    }

    void remove(Entity& entity) {
        auto result = std::find(m_entities.begin(), m_entities.end(), entity);

        if (result != m_entities.end()) {
            m_entities.erase(result);
        }
    }

    [[nodiscard]] std::vector<Entity>& entities() {
        return m_entities;
    }

    [[nodiscard]] const std::vector<Entity>& entities() const {
        return m_entities;
    }
private:
    void updateRegistry(Entity& entity, entt::registry* registry);
};
