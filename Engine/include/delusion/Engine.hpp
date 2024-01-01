#pragma once

#include <cassert>
#include <iostream>
#include <memory>

#include <glfw/glfw3.h>

#include "delusion/AssetManager.hpp"
#include "delusion/graphics/GraphicsBackend.hpp"
#include "delusion/Scene.hpp"
#include "delusion/Window.hpp"

class Engine;

static std::unique_ptr<Engine> s_engine;

class Engine {
    private:
        std::shared_ptr<GraphicsBackend> m_graphicsBackend;
        std::shared_ptr<AssetManager> m_assetManager;
        std::shared_ptr<Window> m_currentWindow;
        std::shared_ptr<Scene> m_currentScene;

        Engine() {
            if (glfwInit() != GLFW_TRUE) {
                throw std::exception("GLFW initialization failed");
            }

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        }
    public:
        ~Engine() {
            glfwTerminate();
        }

        static Engine *get() {
            if (s_engine == nullptr) {
                s_engine = std::unique_ptr<Engine>(new Engine());
            }

            return s_engine.get();
        }

        [[nodiscard]] std::shared_ptr<GraphicsBackend> &graphicsBackend() {
            return m_graphicsBackend;
        }

        void setGraphicsBackend(std::shared_ptr<GraphicsBackend> graphicsBackend) {
            m_graphicsBackend = std::move(graphicsBackend);
        }

        [[nodiscard]] std::shared_ptr<AssetManager> &assetManager() {
            return m_assetManager;
        }

        void setAssetManager(std::shared_ptr<AssetManager> assetManager) {
            m_assetManager = std::move(assetManager);
        }

        [[nodiscard]] Window *currentWindow() const {
            return m_currentWindow.get();
        }

        void setCurrentWindow(std::shared_ptr<Window> window) {
            m_currentWindow = std::move(window);
        }

        [[nodiscard]] Scene *currentScene() const {
            return m_currentScene.get();
        }

        void setCurrentScene(std::shared_ptr<Scene> scene) {
            m_currentScene = std::move(scene);
        }

        void pollEvents() {
            glfwPollEvents();
        }
};
