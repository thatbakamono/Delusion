#pragma once

#include <filesystem>
#include <unordered_map>

#include "delusion/audio/AudioClip.hpp"
#include "delusion/formats/ImageDecoder.hpp"
#include "delusion/graphics/Texture2D.hpp"
#include "delusion/io/FileUtilities.hpp"
#include "delusion/MetadataSerde.hpp"
#include "delusion/UniqueId.hpp"

class AssetManager {
    private:
        WGPUDevice m_device;
        WGPUQueue m_queue;

        std::unordered_map<UniqueId, std::filesystem::path> m_idToPathMappings;
        std::unordered_map<std::filesystem::path, UniqueId> m_pathToIdMappings;

        std::unordered_map<UniqueId, std::shared_ptr<Texture2D>> m_textures;
        std::unordered_map<UniqueId, std::shared_ptr<AudioClip>> m_audioClips;
    public:
        AssetManager(WGPUDevice device, WGPUQueue queue) : m_device(device), m_queue(queue) {}

        [[nodiscard]] bool isLoaded(UniqueId id) {
            return m_textures.contains(id) || m_audioClips.contains(id);
        }

        [[nodiscard]] bool isLoaded(const std::filesystem::path &assetPath) {
            if (m_pathToIdMappings.contains(assetPath)) {
                auto id = m_pathToIdMappings.at(assetPath);

                return m_textures.contains(id) || m_audioClips.contains(id);
            } else {
                return false;
            }
        }

        UniqueId loadAsset(const std::filesystem::path &assetPath) {
            auto metadataPath = assetPath;

            metadataPath.replace_extension(std::format("{}.{}", metadataPath.extension().string(), "metadata"));

            auto metadataContent = readAsString(metadataPath);

            Metadata metadata = MetadataSerde::deserialize(metadataContent.value());

            if (assetPath.extension() == ".png") {
                if (!m_textures.contains(metadata.id)) {
                    auto image = ImageDecoder::decode(assetPath.string());

                    m_textures[metadata.id] = Texture2D::create(metadata.id, m_device, m_queue, image);
                    m_idToPathMappings[metadata.id] = assetPath;
                }
            } else if (assetPath.extension() == ".mp3") {
                if (!m_audioClips.contains(metadata.id)) {
                    m_audioClips[metadata.id] = AudioClip::create(metadata.id, assetPath);
                    m_idToPathMappings[metadata.id] = assetPath;
                }
            }

            return metadata.id;
        }

        void loadAsset(UniqueId id) {
            auto assetPath = m_idToPathMappings.at(id);

            if (assetPath.extension() == ".png") {
                if (!m_textures.contains(id)) {
                    auto image = ImageDecoder::decode(assetPath.string());

                    m_textures[id] = Texture2D::create(id, m_device, m_queue, image);
                }
            } else if (assetPath.extension() == ".mp3") {
                if (!m_audioClips.contains(id)) {
                    m_audioClips[id] = AudioClip::create(id, assetPath);
                    m_idToPathMappings[id] = assetPath;
                }
            }
        }

        void generateMetadataForAllFiles(const std::filesystem::path &rootPath) {
            for (const auto &entry : std::filesystem::directory_iterator(rootPath)) {
                if (!entry.is_directory()) {
                    auto path = entry.path();

                    auto extension = path.extension();

                    if (extension == ".png" || extension == ".mp3") {
                        auto metadataExtension = std::format("{}.{}", path.extension().string(), "metadata");

                        path.replace_extension(metadataExtension);

                        if (!std::filesystem::exists(path)) {
                            std::ofstream stream(path);

                            Metadata metadata;

                            stream << MetadataSerde::serialize(metadata);
                        }
                    }
                } else {
                    generateMetadataForAllFiles(entry.path());
                }
            }
        }

        void generateMetadataForFile(const std::filesystem::path &filePath) {
            auto extension = filePath.extension();

            if (extension == ".png" || extension == ".mp3") {
                auto metadataExtension = std::format("{}.{}", filePath.extension().string(), "metadata");

                auto metadataPath = filePath;

                metadataPath.replace_extension(metadataExtension);

                if (!std::filesystem::exists(metadataPath)) {
                    std::ofstream stream(metadataPath);

                    Metadata metadata;

                    stream << MetadataSerde::serialize(metadata);
                }
            }
        }

        void deleteMetadataOfFile(const std::filesystem::path &filePath) {
            auto metadataExtension = std::format("{}.{}", filePath.extension().string(), "metadata");

            auto metadataPath = filePath;

            metadataPath.replace_extension(metadataExtension);

            std::filesystem::remove(metadataPath);
        }

        void renameMetadataOfFile(const std::filesystem::path &oldPath, const std::filesystem::path &newPath) {
            auto oldMetadataExtension = std::format("{}.{}", oldPath.extension().string(), "metadata");
            auto newMetadataExtension = std::format("{}.{}", newPath.extension().string(), "metadata");

            auto oldMetadataPath = oldPath;

            oldMetadataPath.replace_extension(oldMetadataExtension);

            auto newMetadataPath = newPath;

            newMetadataPath.replace_extension(newMetadataExtension);

            std::filesystem::rename(oldMetadataPath, newMetadataPath);
        }

        void loadMappings(const std::filesystem::path &rootPath) {
            for (const auto &entry : std::filesystem::directory_iterator(rootPath)) {
                if (!entry.is_directory()) {
                    auto path = entry.path();

                    auto extension = path.extension();

                    if (extension == ".png" || extension == ".mp3") {
                        auto metadataExtension = std::format("{}.{}", path.extension().string(), "metadata");

                        auto metadataPath = path;

                        metadataPath.replace_extension(metadataExtension);

                        if (std::filesystem::exists(metadataPath)) {
                            auto content = readAsString(metadataPath);

                            if (content.has_value()) {
                                Metadata metadata = MetadataSerde::deserialize(content.value());

                                m_idToPathMappings[metadata.id] = path;
                                m_pathToIdMappings[path] = metadata.id;
                            }
                        }
                    }
                } else {
                    loadMappings(entry.path());
                }
            }
        }

        void loadMapping(const std::filesystem::path &filePath) {
            auto extension = filePath.extension();

            if (extension == ".png" || extension == ".mp3") {
                auto metadataExtension = std::format("{}.{}", filePath.extension().string(), "metadata");

                auto metadataPath = filePath;

                metadataPath.replace_extension(metadataExtension);

                if (std::filesystem::exists(metadataPath)) {
                    auto content = readAsString(metadataPath);

                    if (content.has_value()) {
                        Metadata metadata = MetadataSerde::deserialize(content.value());

                        m_idToPathMappings[metadata.id] = filePath;
                        m_pathToIdMappings[filePath] = metadata.id;
                    }
                }
            }
        }

        void unloadMapping(UniqueId id) {
            m_pathToIdMappings.erase(m_idToPathMappings.at(id));
            m_idToPathMappings.erase(id);
        }

        void updateMapping(UniqueId id, const std::filesystem::path &path) {
            m_pathToIdMappings.erase(m_idToPathMappings[id]);
            m_idToPathMappings[id] = path;
            m_pathToIdMappings[path] = id;
        }

        [[nodiscard]] UniqueId getIdByPath(const std::filesystem::path &path) const {
            return m_pathToIdMappings.at(path);
        }

        [[nodiscard]] std::shared_ptr<Texture2D> getTextureById(UniqueId id) const {
            return m_textures.at(id);
        }

        [[nodiscard]] std::shared_ptr<AudioClip> getAudioClipById(UniqueId id) const {
            return m_audioClips.at(id);
        }
};
