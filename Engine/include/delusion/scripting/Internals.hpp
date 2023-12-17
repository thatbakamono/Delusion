#pragma once

#include <glm/vec2.hpp>

#include "delusion/Components.hpp"
#include "delusion/Engine.hpp"
#include "delusion/input/Key.hpp"
#include "delusion/UniqueId.hpp"

// Input

bool isKeyDown(Key key) {
    const auto *engine = Engine::get();
    const auto *window = engine->currentWindow();

    return glfwGetKey(window->inner(), static_cast<int>(key)) == GLFW_PRESS;
}

// Transform component

bool hasTransformComponent(UniqueId id) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();

    return entity->hasComponent<TransformComponent>();
}

glm::vec2 getTransformPosition(UniqueId id) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &transform = entity->getComponent<TransformComponent>();

    return transform.position;
}

void setTransformPosition(UniqueId id, glm::vec2 position) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    auto *entity = scene->getById(id).value();
    auto &transform = entity->getComponent<TransformComponent>();

    transform.position = position;
}

glm::vec2 getTransformScale(UniqueId id) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &transform = entity->getComponent<TransformComponent>();

    return transform.scale;
}

void setTransformScale(UniqueId id, glm::vec2 scale) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    auto *entity = scene->getById(id).value();
    auto &transform = entity->getComponent<TransformComponent>();

    transform.scale = scale;
}

float getTransformRotation(UniqueId id) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &transform = entity->getComponent<TransformComponent>();

    return transform.rotation;
}

void setTransformRotation(UniqueId id, float rotation) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    auto *entity = scene->getById(id).value();
    auto &transform = entity->getComponent<TransformComponent>();

    transform.rotation = rotation;
}

// Rigidbody component

bool hasRigidbodyComponent(UniqueId id) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    const auto *entity = scene->getById(id).value();

    return entity->hasComponent<RigidbodyComponent>();
}

RigidbodyComponent::BodyType getRigidbodyBodyType(UniqueId id) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    const auto *entity = scene->getById(id).value();
    const auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    return rigidbody.bodyType;
}

void setRigidbodyBodyType(UniqueId id, RigidbodyComponent::BodyType bodyType) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    auto *entity = scene->getById(id).value();
    auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    rigidbody.bodyType = bodyType;
}

bool getRigidbodyHasFixedRotation(UniqueId id) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    const auto *entity = scene->getById(id).value();
    const auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    return rigidbody.hasFixedRotation;
}

void setRigidbodyHasFixedRotation(UniqueId id, bool hasFixedRotation) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    auto *entity = scene->getById(id).value();
    auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    rigidbody.hasFixedRotation = hasFixedRotation;
}

float getRigidbodyFriction(UniqueId id) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    return rigidbody.friction;
}

void setRigidbodyFriction(UniqueId id, float friction) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    auto *entity = scene->getById(id).value();
    auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    rigidbody.friction = friction;
}

float getRigidbodyDensity(UniqueId id) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    return rigidbody.density;
}

void setRigidbodyDensity(UniqueId id, float density) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    auto *entity = scene->getById(id).value();
    auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    rigidbody.density = density;
}

float getRigidbodyRestitution(UniqueId id) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    return rigidbody.restitution;
}

void setRigidbodyRestitution(UniqueId id, float restitution) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    auto *entity = scene->getById(id).value();
    auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    rigidbody.restitution = restitution;
}

float getRigidbodyRestitutionThreshold(UniqueId id) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &rigidbody = entity->getComponent<RigidbodyComponent>();

    return rigidbody.restitutionThreshold;
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

bool hasBoxColliderComponent(UniqueId id) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    const auto *entity = scene->getById(id).value();

    return entity->hasComponent<BoxColliderComponent>();
}

glm::vec2 getBoxColliderSize(UniqueId id) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &boxCollider = entity->getComponent<BoxColliderComponent>();

    return boxCollider.size;
}

void setBoxColliderSize(UniqueId id, glm::vec2 size) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    auto *entity = scene->getById(id).value();
    auto &boxCollider = entity->getComponent<BoxColliderComponent>();

    boxCollider.size = size;
}

glm::vec2 getBoxColliderOffset(UniqueId id) {
    const auto *engine = Engine::get();
    const auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    const auto *entity = scene->getById(id).value();
    const auto &boxCollider = entity->getComponent<BoxColliderComponent>();

    return boxCollider.offset;
}

void setBoxColliderOffset(UniqueId id, glm::vec2 offset) {
    auto *engine = Engine::get();
    auto *scene = engine->currentScene();
    // NOTE: This shouldn't fail unless there's somewhere a really serious bug
    auto *entity = scene->getById(id).value();
    auto &boxCollider = entity->getComponent<BoxColliderComponent>();

    boxCollider.offset = offset;
}
