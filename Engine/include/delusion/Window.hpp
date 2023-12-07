#pragma once

#include <string>
#include <memory>

#include <GLFW/glfw3.h>
#include <webgpu.h>

class Window {
private:
    std::unique_ptr<GLFWwindow, decltype(glfwDestroyWindow) *> m_window;
public:
    Window(const std::string& title, uint32_t width, uint32_t height)
        : m_window(glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr), &glfwDestroyWindow) {}

    [[nodiscard]] bool isOpen() const {
        return !glfwWindowShouldClose(m_window.get());
    }

    [[nodiscard]] GLFWwindow* inner() const {
        return m_window.get();
    }
};
