#pragma once

#include <glm/ext.hpp>
#include <glm/glm.hpp>

class OrthographicCamera {
    private:
        glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);

        float m_aspectRatio = 1.0f;

        float m_zNear = 0.1f;
        float m_zFar = 100.0f;

        float m_zoom = 1.0f;
    public:
        explicit OrthographicCamera(glm::vec3 position) : m_position(position) {}

        [[nodiscard]] glm::vec3 position() const {
            return m_position;
        }

        void setPosition(glm::vec3 position) {
            m_position = position;
        }

        [[nodiscard]] float aspectRatio() const {
            return m_aspectRatio;
        }

        void setAspectRatio(float aspectRatio) {
            m_aspectRatio = aspectRatio;
        }

        [[nodiscard]] float zoom() const {
            return m_zoom;
        }

        void setZoom(float zoom) {
            m_zoom = zoom;
        }

        [[nodiscard]] glm::mat4 viewMatrix() const {
            return glm::lookAt(m_position, m_position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        [[nodiscard]] glm::mat4 projectionMatrix() const {
            return glm::ortho(
                -m_aspectRatio * m_zoom, m_aspectRatio * m_zoom, -1.0f * m_zoom, 1.0f * m_zoom, m_zNear, m_zFar
            );
        }

        [[nodiscard]] glm::mat4 viewProjectionMatrix() const {
            return projectionMatrix() * viewMatrix();
        }
};
