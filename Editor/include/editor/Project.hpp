#pragma once

#include <filesystem>

class Project {
    private:
        std::filesystem::path m_path;
        std::filesystem::path m_assetsDirectoryPath;
    public:
        explicit Project(std::filesystem::path path) : m_path(std::move(path)) {
            m_assetsDirectoryPath = m_path / "assets";
        }

        [[nodiscard]] const std::filesystem::path &path() const {
            return m_path;
        }

        [[nodiscard]] const std::filesystem::path &assetsDirectoryPath() const {
            return m_assetsDirectoryPath;
        }
};
