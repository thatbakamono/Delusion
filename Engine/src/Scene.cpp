#include "delusion/Scene.hpp"

#include "delusion/Components.hpp"

Scene::Scene(Scene &&other) noexcept {
    m_entities = std::move(other.m_entities);

    m_registry = std::move(other.m_registry);

    for (auto &entity : m_entities) {
        updateRegistry(entity, &m_registry);
    }

    m_physicsWorld = std::move(other.m_physicsWorld);
}

Scene &Scene::operator=(Scene &&other) noexcept {
    m_entities = std::move(other.m_entities);

    m_registry = std::move(other.m_registry);

    for (auto &entity : m_entities) {
        updateRegistry(entity, &m_registry);
    }

    m_physicsWorld = std::move(other.m_physicsWorld);
    m_lua = std::move(other.m_lua);

    return *this;
}

Scene Scene::copy(Scene &source) {
    Scene copiedScene;

    for (auto &entity : source.entities()) {
        auto &copiedEntity = copiedScene.create();

        copyEntity(copiedEntity, entity);
    }

    return copiedScene;
}

void Scene::start(const std::filesystem::path &assetsDirectory) {
    m_physicsWorld = std::unique_ptr<b2World>(new b2World({ 0.0f, -10.0f }));
    m_lua = std::make_unique<Lua>();

    auto globals = m_lua->globals();
    auto packageTable = globals.get("package").value();

    auto workingDirectory = std::filesystem::current_path();
    auto workingDirectoryString = workingDirectory.string();
    auto assetsDirectoryString = assetsDirectory.string();

    packageTable.set("path", std::format("{}/?.lua;{}/?.lua", workingDirectoryString, assetsDirectoryString));

    {
        auto view = m_registry.view<ScriptComponent>();

        for (auto entity : view) {
            auto &scriptComponent = view.get<ScriptComponent>(entity);
            auto table = m_lua->load(scriptComponent.script->name(), scriptComponent.script->source());

            if (table.has_value()) {
                scriptComponent.script->setTable(std::make_unique<Table>(std::move(table.value())));
            }
        }
    }

    {
        auto view = m_registry.view<ScriptComponent>();

        for (auto entity : view) {
            auto &scriptComponent = view.get<ScriptComponent>(entity);
            auto *table = scriptComponent.script->table();

            if (table != nullptr) {
                table->call("on_start");
            }
        }
    }

    {
        auto view = m_registry.view<TransformComponent, RigidbodyComponent>();

        for (auto entity : view) {
            auto [transform, rigidbody] = view.get<TransformComponent, RigidbodyComponent>(entity);

            b2BodyType bodyType {};

            switch (rigidbody.bodyType) {
                case RigidbodyComponent::BodyType::Static:
                    bodyType = b2BodyType::b2_staticBody;

                    break;
                case RigidbodyComponent::BodyType::Dynamic:
                    bodyType = b2BodyType::b2_dynamicBody;

                    break;
                case RigidbodyComponent::BodyType::Kinematic:
                    bodyType = b2BodyType::b2_kinematicBody;

                    break;
                default:
                    assert(false);

                    break;
            }

            b2BodyDef bodyDefinition {};
            bodyDefinition.type = bodyType;
            bodyDefinition.fixedRotation = rigidbody.hasFixedRotation;
            bodyDefinition.position.Set(transform.position.x, transform.position.y);
            bodyDefinition.angle = transform.rotation;

            b2Body *body = m_physicsWorld->CreateBody(&bodyDefinition);

            rigidbody.body = static_cast<void *>(body);

            b2FixtureDef fixtureDefinition {};
            fixtureDefinition.density = rigidbody.density;
            fixtureDefinition.friction = rigidbody.friction;
            fixtureDefinition.restitution = rigidbody.restitution;
            fixtureDefinition.restitutionThreshold = rigidbody.restitutionThreshold;

            b2Fixture *fixture;

            if (m_registry.any_of<BoxColliderComponent>(entity)) {
                auto &collider = m_registry.get<BoxColliderComponent>(entity);

                b2PolygonShape boxShape;
                boxShape.SetAsBox(
                    transform.scale.x * collider.size.x * 0.5f, transform.scale.y * collider.size.y * 0.5f
                );

                fixtureDefinition.shape = &boxShape;

                fixture = body->CreateFixture(&fixtureDefinition);
            } else {
                fixture = body->CreateFixture(&fixtureDefinition);
            }

            rigidbody.fixture = static_cast<void *>(fixture);
        }
    }
}

void Scene::stop() {
    {
        auto view = m_registry.view<ScriptComponent>();

        for (auto entity : view) {
            auto &scriptComponent = view.get<ScriptComponent>(entity);

            scriptComponent.script->resetTable();
        }
    }

    m_lua.reset();
    m_physicsWorld.reset();
}

void Scene::onUpdate(float deltaTime) {
    {
        auto view = m_registry.view<ScriptComponent>();

        for (auto entity : view) {
            auto &script = view.get<ScriptComponent>(entity);
            auto *table = script.script->table();

            if (table != nullptr) {
                table->call("on_update");
            }
        }
    }

    {
        // The suggested iteration count for Box2D is 8 for velocity and 3 for position.
        // You can tune this number to your liking,
        // just keep in mind that this has a trade-off between performance and accuracy.
        // ~ https://box2d.org/documentation/md__d_1__git_hub_box2d_docs_hello.html#autotoc_md24
        constexpr int32_t velocityIterations = 8;
        constexpr int32_t positionIterations = 3;

        m_physicsWorld->Step(deltaTime, velocityIterations, positionIterations);

        auto view = m_registry.view<TransformComponent, RigidbodyComponent>();

        for (auto entity : view) {
            auto [transform, rigidbody] = view.get<TransformComponent, RigidbodyComponent>(entity);

            b2Body *body = static_cast<b2Body *>(rigidbody.body);

            const auto &position = body->GetPosition();

            transform.position.x = position.x;
            transform.position.y = position.y;
            transform.rotation = body->GetAngle();
        }
    }
}

Entity &Scene::create() {
    auto entityId = m_registry.create();

    m_entities.emplace_back(&m_registry, entityId);

    return m_entities.back();
}

void Scene::remove(Entity &entity) {
    auto result = std::find(m_entities.begin(), m_entities.end(), entity);

    if (result != m_entities.end()) {
        m_entities.erase(result);
    }
}

std::optional<Entity *> Scene::getById(UniqueId id) {
    for (auto &entity : m_entities) {
        if (entity.id() == id) {
            return std::make_optional(&entity);
        }
    }

    for (auto &entity : m_entities) {
        auto result = getById(entity, id);

        if (result.has_value()) {
            return result;
        }
    }

    return std::nullopt;
}

std::optional<Entity *> Scene::getById(Entity &parent, UniqueId id) {
    for (auto &child : parent.children()) {
        if (child.id() == id) {
            return std::make_optional(&child);
        }
    }

    for (auto &child : parent.children()) {
        auto result = getById(child, id);

        if (result.has_value()) {
            return result;
        }
    }

    return std::nullopt;
}

void Scene::copyEntity(Entity &target, Entity &source) {
    target.m_id = source.m_id;

    if (source.hasComponent<TransformComponent>()) {
        auto &transform = source.getComponent<TransformComponent>();

        target.addComponent<TransformComponent>(transform);
    }

    if (source.hasComponent<SpriteComponent>()) {
        auto &sprite = source.getComponent<SpriteComponent>();

        target.addComponent<SpriteComponent>(sprite);
    }

    if (source.hasComponent<RigidbodyComponent>()) {
        auto &rigidbody = source.getComponent<RigidbodyComponent>();

        target.addComponent<RigidbodyComponent>(rigidbody);
    }

    if (source.hasComponent<BoxColliderComponent>()) {
        auto &collider = source.getComponent<BoxColliderComponent>();

        target.addComponent<BoxColliderComponent>(collider);
    }

    if (source.hasComponent<ScriptComponent>()) {
        auto &script = source.getComponent<ScriptComponent>();

        target.addComponent<ScriptComponent>(script);
    }

    for (auto &child : source.children()) {
        copyEntity(target.createChild(), child);
    }
}

void Scene::updateRegistry(Entity &entity, entt::registry *registry) {
    entity.m_registry = &m_registry;

    for (auto &child : entity.children()) {
        child.m_registry = &m_registry;

        updateRegistry(child, registry);
    }
}
