module;

#include <vector>

#include <entt/entt.hpp>

export module scene;

import entity;

export class Scene {
private:
    entt::registry m_registry;

    std::vector<Entity> m_entities;
public:
    Scene(const Scene& other) = delete;
    Scene(Scene&& other) noexcept = delete;

    Scene& operator =(const Scene& other) = delete;
    Scene& operator =(Scene&& other) noexcept = delete;

    Entity& create() {
        auto entityId = m_registry.create();

        m_entities.emplace_back(m_registry, entityId);

        return m_entities.back();
    }

    void remove(const Entity& entity) {
        m_entities.erase(std::find(m_entities.begin(), m_entities.end(), entity));
    }

    [[nodiscard]] const std::vector<Entity>& entities() const {
        return m_entities;
    }
};
