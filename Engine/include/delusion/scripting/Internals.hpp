#pragma once

#include <glm/vec2.hpp>

#include "delusion/Components.hpp"
#include "delusion/Engine.hpp"
#include "delusion/input/Key.hpp"
#include "delusion/UniqueId.hpp"

// Input

void isKeyDown(Key key, bool *result) {
    const auto *engine = Engine::get();
    const auto *window = engine->currentWindow();

    *result = glfwGetKey(window->inner(), static_cast<int>(key)) == GLFW_PRESS;
}

// Transform component

void hasTransformComponent(UniqueId id, bool *result) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();

    *result = entity->hasComponent<TransformComponent>();
}

void getTransformPosition(UniqueId id, glm::vec2 *result) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &transform = entity->getComponent<TransformComponent>();

    *result = transform.position;
}

void setTransformPosition(UniqueId id, glm::vec2 position) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    auto *entity = scene->getById(id).value();
    auto &transform = entity->getComponent<TransformComponent>();

    transform.position = position;
}

void getTransformScale(UniqueId id, glm::vec2 *result) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &transform = entity->getComponent<TransformComponent>();

    *result = transform.scale;
}

void setTransformScale(UniqueId id, glm::vec2 scale) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    auto *entity = scene->getById(id).value();
    auto &transform = entity->getComponent<TransformComponent>();

    transform.scale = scale;
}

void getTransformRotation(UniqueId id, float *result) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &transform = entity->getComponent<TransformComponent>();

    *result = transform.rotation;
}

void setTransformRotation(UniqueId id, float rotation) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    auto *entity = scene->getById(id).value();
    auto &transform = entity->getComponent<TransformComponent>();

    transform.rotation = rotation;
}

// Sprite component

void hasSpriteComponent(UniqueId id, bool *result) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    const auto *entity = scene->getById(id).value();

    *result = entity->hasComponent<SpriteComponent>();
}

void getSpriteTexture(UniqueId id, UniqueId *result) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    const auto *entity = scene->getById(id).value();
    const auto &sprite = entity->getComponent<SpriteComponent>();

    *result = sprite.texture->id();
}

void setSpriteTexture(UniqueId id, UniqueId textureId) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    auto *entity = scene->getById(id).value();
    auto &sprite = entity->getComponent<SpriteComponent>();

    sprite.texture = engine->assetManager()->getTextureById(textureId);
}

// Rigidbody component

void hasRigidbodyComponent(UniqueId id, bool *result) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    const auto *entity = scene->getById(id).value();

    *result = entity->hasComponent<RigidbodyComponent>();
}

void getRigidbodyBodyType(UniqueId id, RigidbodyComponent::BodyType *result) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    const auto *entity = scene->getById(id).value();
    const auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    *result = rigidbody.bodyType;
}

void setRigidbodyBodyType(UniqueId id, RigidbodyComponent::BodyType bodyType) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    auto *entity = scene->getById(id).value();
    auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    rigidbody.bodyType = bodyType;
}

void getRigidbodyHasFixedRotation(UniqueId id, bool *result) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    const auto *entity = scene->getById(id).value();
    const auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    *result = rigidbody.hasFixedRotation;
}

void setRigidbodyHasFixedRotation(UniqueId id, bool hasFixedRotation) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    auto *entity = scene->getById(id).value();
    auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    rigidbody.hasFixedRotation = hasFixedRotation;
}

void getRigidbodyFriction(UniqueId id, float *result) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    *result = rigidbody.friction;
}

void setRigidbodyFriction(UniqueId id, float friction) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    auto *entity = scene->getById(id).value();
    auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    rigidbody.friction = friction;
}

void getRigidbodyDensity(UniqueId id, float *result) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    *result = rigidbody.density;
}

void setRigidbodyDensity(UniqueId id, float density) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    auto *entity = scene->getById(id).value();
    auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    rigidbody.density = density;
}

void getRigidbodyRestitution(UniqueId id, float *result) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    *result = rigidbody.restitution;
}

void setRigidbodyRestitution(UniqueId id, float restitution) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    auto *entity = scene->getById(id).value();
    auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    rigidbody.restitution = restitution;
}

void getRigidbodyRestitutionThreshold(UniqueId id, float *result) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    *result = rigidbody.restitutionThreshold;
}

void setRigidbodyRestitutionThreshold(UniqueId id, float restitutionThreshold) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    auto *entity = scene->getById(id).value();
    auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    rigidbody.restitutionThreshold = restitutionThreshold;
}

// Box collider component

void hasBoxColliderComponent(UniqueId id, bool *result) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    const auto *entity = scene->getById(id).value();

    *result = entity->hasComponent<BoxColliderComponent>();
}

void getBoxColliderSize(UniqueId id, glm::vec2 *result) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &boxCollider = entity->getComponent<BoxColliderComponent>();

    *result = boxCollider.size;
}

void setBoxColliderSize(UniqueId id, glm::vec2 size) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    auto *entity = scene->getById(id).value();
    auto &boxCollider = entity->getComponent<BoxColliderComponent>();

    boxCollider.size = size;
}

void getBoxColliderOffset(UniqueId id, glm::vec2 *result) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &boxCollider = entity->getComponent<BoxColliderComponent>();

    *result = boxCollider.offset;
}

void setBoxColliderOffset(UniqueId id, glm::vec2 offset) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    auto *entity = scene->getById(id).value();
    auto &boxCollider = entity->getComponent<BoxColliderComponent>();

    boxCollider.offset = offset;
}
