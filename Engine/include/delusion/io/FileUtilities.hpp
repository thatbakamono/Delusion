#pragma once

#include <filesystem>
#include <fstream>
#include <optional>

[[nodiscard]] static std::optional<std::string> readAsString(const std::filesystem::path& path)
{
    std::ifstream fileStream(path);

    if (fileStream.good())
    {
        auto result = std::make_optional<std::string>();

        fileStream.seekg(0, std::ios::end);
        result->resize(fileStream.tellg());
        fileStream.seekg(0, std::ios::beg);
        result->assign(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());

        return result;
    }

    return {};
}
