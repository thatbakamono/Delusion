#pragma once

#include <span>

#include <entt/entt.hpp>

#include "delusion/UniqueId.hpp"

class Scene;

class Entity {
    private:
        entt::registry *m_registry;
        entt::entity m_entityId;

        UniqueId m_id;

        std::vector<Entity> m_children;
    public:
        Entity(entt::registry *registry, const entt::entity entityId)
            : m_registry(registry), m_entityId(entityId), m_id(UniqueId()) {}

        Entity(entt::registry *registry, const entt::entity entityId, UniqueId id)
            : m_registry(registry), m_entityId(entityId), m_id(id) {}

        // TODO: Implement
        Entity(const Entity &other) = delete;

        Entity(Entity &&other) noexcept {
            m_registry = std::exchange(other.m_registry, nullptr);
            m_entityId = std::exchange(other.m_entityId, entt::null);
            m_id = std::exchange(other.m_id, UniqueId(0));
            m_children = std::move(other.m_children);
        }

        ~Entity() {
            if (m_entityId != entt::null) {
                m_registry->destroy(m_entityId);
            }
        }

        // TODO: Implement
        Entity &operator=(const Entity &other) = delete;

        Entity &operator=(Entity &&other) noexcept {
            if (m_entityId != entt::null) {
                m_registry->destroy(m_entityId);
            }

            m_registry = std::exchange(other.m_registry, nullptr);
            m_entityId = std::exchange(other.m_entityId, entt::null);
            m_id = std::exchange(other.m_id, UniqueId(0));
            m_children = std::move(other.m_children);

            return *this;
        }

        [[nodiscard]] bool operator==(const Entity &other) noexcept {
            return &m_registry == &other.m_registry && m_entityId == other.m_entityId;
        }

        [[nodiscard]] bool operator!=(const Entity &other) noexcept {
            return &m_registry != &other.m_registry || m_entityId != other.m_entityId;
        }

        [[nodiscard]] UniqueId id() const {
            return m_id;
        }

        Entity &createChild() {
            auto entityId = m_registry->create();

            m_children.emplace_back(m_registry, entityId);

            return m_children.back();
        }

        void removeChild(Entity &entity) {
            auto result = std::find(m_children.begin(), m_children.end(), entity);

            if (result != m_children.end()) {
                m_children.erase(result);
            }
        }

        [[nodiscard]] std::vector<Entity> &children() {
            return m_children;
        }

        [[nodiscard]] const std::vector<Entity> &children() const {
            return m_children;
        }

        template <typename T>
        [[nodiscard]] bool hasComponent() const {
            return m_registry->all_of<T>(m_entityId);
        }

        template <typename T, typename... Parameters>
        void addComponent(Parameters &&...parameters) {
            if (hasComponent<T>()) {
                throw std::runtime_error("Entity already has component");
            }

            m_registry->emplace<T>(m_entityId, std::forward<Parameters>(parameters)...);
        }

        template <typename T>
        void removeComponent() {
            if (!hasComponent<T>()) {
                throw std::runtime_error("Entity doesn't have component");
            }

            m_registry->remove<T>(m_entityId);
        }

        template <typename T>
        T &getComponent() {
            if (!hasComponent<T>()) {
                throw std::runtime_error("Entity doesn't have component");
            }

            return m_registry->get<T>(m_entityId);
        }

        template <typename T>
        const T &getComponent() const {
            if (!hasComponent<T>()) {
                throw std::runtime_error("Entity doesn't have component");
            }

            return m_registry->get<T>(m_entityId);
        }

        friend Scene;
};
