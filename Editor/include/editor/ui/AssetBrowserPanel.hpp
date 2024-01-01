#pragma once

#include <filesystem>

#include <imgui.h>

#include "editor/Project.hpp"

#include "delusion/graphics/Texture2D.hpp"

class AssetBrowserPanel {
    private:
        std::shared_ptr<Texture2D> m_fileIconTexture;
        std::shared_ptr<Texture2D> m_directoryIconTexture;

        std::filesystem::path m_currentDirectory;
    public:
        AssetBrowserPanel(std::shared_ptr<Texture2D> fileIconTexture, std::shared_ptr<Texture2D> directoryIconTexture)
            : m_fileIconTexture(std::move(fileIconTexture)), m_directoryIconTexture(std::move(directoryIconTexture)) {}

        void onUpdate(const Project &project);

        [[nodiscard]] const std::filesystem::path &currentDirectory() const {
            return m_currentDirectory;
        }

        void setCurrentDirectory(const std::filesystem::path &currentDirectory) {
            m_currentDirectory = currentDirectory;
        }
};
