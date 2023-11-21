module;

#include <span>
#include <vector>

#include <entt/entt.hpp>

export module entity;

export class Entity {
private:
    entt::registry& m_registry;
    entt::entity m_entityId;

    std::vector<Entity> m_children;
public:
    Entity(entt::registry& registry, const entt::entity entityId) : m_registry(registry), m_entityId(entityId) {}

    // TODO: Implement
    Entity(const Entity& other) = delete;

    Entity(Entity&& other) noexcept : m_registry(other.m_registry) {
        m_entityId = std::exchange(other.m_entityId, entt::null);
        m_children = std::move(other.m_children);
    }

    ~Entity() {
        if (m_entityId != entt::null) {
            m_registry.destroy(m_entityId);
        }
    }

    // TODO: Implement
    Entity& operator =(const Entity& other) = delete;

    Entity& operator =(Entity&& other) noexcept {
        if (m_entityId != entt::null) {
            m_registry.destroy(m_entityId);
        }

        m_registry = std::move(other.m_registry);
        m_entityId = std::exchange(other.m_entityId, entt::null);
        m_children = std::move(other.m_children);

        return *this;
    }

    [[nodiscard]] bool operator ==(const Entity& other) noexcept {
        return &m_registry == &other.m_registry && m_entityId == other.m_entityId;
    }

    Entity& createChild() {
        auto entityId = m_registry.create();

        m_children.emplace_back(m_registry, entityId);

        return m_children.back();
    }

    void removeChild(Entity& entity) {
        m_children.erase(std::find(m_children.begin(), m_children.end(), entity));
    }

    [[nodiscard]] std::span<Entity> children() {
        return { m_children };
    }

    template<typename T>
    [[nodiscard]] bool hasComponent() const {
        return m_registry.all_of<T>(m_entityId);
    }

    template<typename T, typename... Parameters>
    void addComponent(Parameters&&... parameters) {
        if (hasComponent<T>()) {
            throw std::runtime_error("Entity already has component");
        }

        return m_registry.emplace<T>(m_entityId, std::forward<Parameters>(parameters)...);
    }

    template<typename T>
    void removeComponent() {
        if (!hasComponent<T>()) {
            throw std::runtime_error("Entity doesn't have component");
        }

        m_registry.remove<T>(m_entityId);
    }

    template<typename T>
    T& getComponent() {
        if (!hasComponent<T>()) {
            throw std::runtime_error("Entity doesn't have component");
        }

        return m_registry.get<T>(m_entityId);
    }

    template<typename T>
    const T& getComponent() const {
        if (!hasComponent<T>()) {
            throw std::runtime_error("Entity doesn't have component");
        }

        return m_registry.get<T>(m_entityId);
    }
};
