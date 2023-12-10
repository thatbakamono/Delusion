#include "delusion/SceneSerde.hpp"

#include "delusion/Components.hpp"

Scene SceneSerde::deserialize(const std::string &input) {
    Scene scene;

    YAML::Node node = YAML::Load(input);
    auto entitiesNode = node["entities"];

    for (size_t entityIndex = 0; entityIndex < entitiesNode.size(); entityIndex++) {
        auto entityNode = entitiesNode[entityIndex];

        auto &entity = scene.create();

        deserializeEntity(entityNode, entity);
    }

    return scene;
}

std::string SceneSerde::serialize(const Scene &scene) {
    YAML::Emitter emitter;

    emitter << YAML::BeginMap;

    emitter << YAML::Key << "entities";
    emitter << YAML::BeginSeq;

    for (const auto &entity : scene.entities()) {
        serializeEntity(emitter, entity);
    }

    emitter << YAML::EndSeq;

    emitter << YAML::EndMap;

    return { emitter.c_str() };
}

void SceneSerde::deserializeEntity(YAML::Node &entityNode, Entity &entity) {
    // TODO: Implement some kind of component registry with metadata, so it doesn't have to be done manually
    auto componentsNode = entityNode["components"];

    if (componentsNode) {
        auto transformNode = componentsNode["transform"];

        if (transformNode) {
            auto positionNode = transformNode["position"];
            auto rotationNode = transformNode["rotation"];
            auto scaleNode = transformNode["scale"];

            glm::vec2 position(positionNode["x"].as<float>(), positionNode["y"].as<float>());
            auto rotation = rotationNode.as<float>();
            glm::vec2 scale(scaleNode["width"].as<float>(), scaleNode["height"].as<float>());

            entity.addComponent<Transform>(position, scale, rotation);
        }

        auto spriteNode = componentsNode["sprite"];

        if (spriteNode) {
            auto idNode = spriteNode["id"];
            auto id = UniqueId(idNode.as<uint64_t>());

            if (!m_assetManager->isLoaded(id)) {
                m_assetManager->loadAsset(id);
            }

            auto texture = m_assetManager->getTextureById(id);

            entity.addComponent<Sprite>(texture);
        }

        auto rigidbodyNode = componentsNode["rigidbody"];

        if (rigidbodyNode) {
            auto bodyTypeText = rigidbodyNode["body-type"].as<std::string>();

            Rigidbody::BodyType bodyType {};

            if (bodyTypeText == "static") {
                bodyType = Rigidbody::BodyType::Static;
            } else if (bodyTypeText == "dynamic") {
                bodyType = Rigidbody::BodyType::Dynamic;
            } else if (bodyTypeText == "kinematic") {
                bodyType = Rigidbody::BodyType::Kinematic;
            } else {
                assert(false);
            }

            auto hasFixedRotation = rigidbodyNode["has-fixed-rotation"].as<bool>();
            auto density = rigidbodyNode["density"].as<float>();
            auto friction = rigidbodyNode["friction"].as<float>();
            auto restitution = rigidbodyNode["restitution"].as<float>();
            auto restitutionThreshold = rigidbodyNode["restitution-threshold"].as<float>();

            Rigidbody rigidbody = {};
            rigidbody.bodyType = bodyType;
            rigidbody.hasFixedRotation = hasFixedRotation;
            rigidbody.density = density;
            rigidbody.friction = friction;
            rigidbody.restitution = restitution;
            rigidbody.restitutionThreshold = restitutionThreshold;

            entity.addComponent<Rigidbody>(rigidbody);
        }

        auto boxColliderNode = componentsNode["box-collider"];

        if (boxColliderNode) {
            auto sizeNode = boxColliderNode["size"];
            auto offsetNode = boxColliderNode["offset"];

            glm::vec2 size = { sizeNode["width"].as<float>(), sizeNode["height"].as<float>() };
            glm::vec2 offset = { offsetNode["x"].as<float>(), offsetNode["y"].as<float>() };

            entity.addComponent<BoxCollider>(size, offset);
        }
    }

    auto childrenNode = entityNode["children"];

    if (childrenNode) {
        for (size_t childIndex = 0; childIndex < childrenNode.size(); childIndex++) {
            auto &childEntity = entity.createChild();
            auto childNode = childrenNode[childIndex];

            deserializeEntity(childNode, childEntity);
        }
    }
}

void SceneSerde::serializeEntity(YAML::Emitter &emitter, const Entity &entity) {
    emitter << YAML::BeginMap;

    // TODO: Implement some kind of component registry with metadata, so it doesn't have to be done manually
    if (entity.hasComponent<Transform>() || entity.hasComponent<Sprite>() || entity.hasComponent<Rigidbody>() ||
        entity.hasComponent<BoxCollider>()) {
        emitter << YAML::Key << "components";
        emitter << YAML::BeginMap;

        if (entity.hasComponent<Transform>()) {
            const auto &transform = entity.getComponent<Transform>();

            emitter << YAML::Key << "transform";
            emitter << YAML::BeginMap;

            emitter << YAML::Key << "position";
            emitter << YAML::BeginMap;

            emitter << YAML::Key << "x";
            emitter << YAML::Value << transform.position.x;

            emitter << YAML::Key << "y";
            emitter << YAML::Value << transform.position.y;

            emitter << YAML::EndMap;

            emitter << YAML::Key << "rotation";
            emitter << YAML::Value << transform.rotation;

            emitter << YAML::Key << "scale";
            emitter << YAML::BeginMap;

            emitter << YAML::Key << "width";
            emitter << YAML::Value << transform.scale.x;

            emitter << YAML::Key << "height";
            emitter << YAML::Value << transform.scale.y;

            emitter << YAML::EndMap;

            emitter << YAML::EndMap;
        }

        if (entity.hasComponent<Sprite>()) {
            const auto &sprite = entity.getComponent<Sprite>();

            emitter << YAML::Key << "sprite";
            emitter << YAML::BeginMap;

            emitter << YAML::Key << "id";
            emitter << YAML::Value << sprite.texture->id().value();

            emitter << YAML::EndMap;
        }

        if (entity.hasComponent<Rigidbody>()) {
            const auto &rigidbody = entity.getComponent<Rigidbody>();

            emitter << YAML::Key << "rigidbody";
            emitter << YAML::BeginMap;

            emitter << YAML::Key << "body-type";

            switch (rigidbody.bodyType) {

                case Rigidbody::BodyType::Static:
                    emitter << YAML::Value << "static";

                    break;
                case Rigidbody::BodyType::Dynamic:
                    emitter << YAML::Value << "dynamic";

                    break;
                case Rigidbody::BodyType::Kinematic:
                    emitter << YAML::Value << "kinematic";

                    break;
            }

            emitter << YAML::Key << "has-fixed-rotation";
            emitter << YAML::Value << rigidbody.hasFixedRotation;

            emitter << YAML::Key << "density";
            emitter << YAML::Value << rigidbody.density;

            emitter << YAML::Key << "friction";
            emitter << YAML::Value << rigidbody.friction;

            emitter << YAML::Key << "restitution";
            emitter << YAML::Value << rigidbody.restitution;

            emitter << YAML::Key << "restitution-threshold";
            emitter << YAML::Value << rigidbody.restitutionThreshold;

            emitter << YAML::EndMap;
        }

        if (entity.hasComponent<BoxCollider>()) {
            const auto &collider = entity.getComponent<BoxCollider>();

            emitter << YAML::Key << "box-collider";
            emitter << YAML::BeginMap;

            emitter << YAML::Key << "size";
            emitter << YAML::BeginMap;

            emitter << YAML::Key << "width";
            emitter << YAML::Value << collider.size.x;

            emitter << YAML::Key << "height";
            emitter << YAML::Value << collider.size.y;

            emitter << YAML::EndMap;

            emitter << YAML::Key << "offset";
            emitter << YAML::BeginMap;

            emitter << YAML::Key << "x";
            emitter << YAML::Value << collider.offset.x;

            emitter << YAML::Key << "y";
            emitter << YAML::Value << collider.offset.y;

            emitter << YAML::EndMap;

            emitter << YAML::EndMap;
        }

        emitter << YAML::EndMap;
    }

    emitter << YAML::Key << "children";
    emitter << YAML::BeginSeq;

    for (const auto &child : entity.children()) {
        serializeEntity(emitter, child);
    }

    emitter << YAML::EndSeq;

    emitter << YAML::EndMap;
}
