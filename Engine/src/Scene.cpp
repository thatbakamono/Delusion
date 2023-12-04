#include "delusion/Scene.hpp"

Scene::Scene(Scene &&other) noexcept {
    m_entities = std::move(other.m_entities);

    m_registry = std::move(other.m_registry);

    for (auto& entity : m_entities) {
        updateRegistry(entity, &m_registry);
    }
}

Scene &Scene::operator=(Scene &&other) noexcept {
    m_entities = std::move(other.m_entities);

    m_registry = std::move(other.m_registry);

    for (auto& entity : m_entities) {
        updateRegistry(entity, &m_registry);
    }

    return *this;
}

void Scene::updateRegistry(Entity &entity, entt::registry *registry) {
    entity.m_registry = &m_registry;

    for (auto& child : entity.children()) {
        child.m_registry = &m_registry;

        updateRegistry(child, registry);
    }
}
