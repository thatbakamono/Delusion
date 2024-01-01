#include "editor/ui/AssetBrowserPanel.hpp"

void AssetBrowserPanel::onUpdate(const Project &project) {
    ImGui::Begin("Asset browser");

    if (m_currentDirectory != project.assetsDirectoryPath()) {
        if (ImGui::Button("<-")) {
            m_currentDirectory = m_currentDirectory.parent_path();
        }
    }

    auto availableSize = ImGui::GetContentRegionAvail();

    constexpr float padding = 8.0f;
    constexpr float thumbnailSize = 64.0f;

    auto totalSize = thumbnailSize + padding;
    auto columnCount = static_cast<int>(availableSize.x / totalSize);

    if (columnCount < 1)
        columnCount = 1;

    ImGui::Columns(columnCount, nullptr, false);

    for (const auto &entry : std::filesystem::directory_iterator(m_currentDirectory)) {
        const auto &path = entry.path();

        auto extension = path.filename().extension();
        auto extensionText = extension.string();

        if (extensionText == ".metadata") {
            continue;
        }

        // TODO: Support for non-ascii characters
        std::string name = path.filename().string();

        auto iconTexture = entry.is_directory() ? m_directoryIconTexture : m_fileIconTexture;

        ImGui::PushID(name.c_str());

        ImGui::ImageButton(iconTexture->view(), { thumbnailSize, thumbnailSize }, { 0.0f, 1.0f }, { 1.0f, 0.0f });

        if (!entry.is_directory()) {
            // TODO: Ask some kind of FormatRegistry whether it's a supported image format
            if (extensionText == ".png") {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                    auto pathText = path.string();

                    ImGui::SetDragDropPayload("image", pathText.c_str(), pathText.size() + 1, ImGuiCond_Once);

                    ImGui::EndDragDropSource();
                }
            }

            if (extensionText == ".scene") {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                    auto pathText = path.string();

                    ImGui::SetDragDropPayload("scene", pathText.c_str(), pathText.size() + 1, ImGuiCond_Once);

                    ImGui::EndDragDropSource();
                }
            }
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            if (entry.is_directory()) {
                m_currentDirectory /= path.filename();
            }
        }

        ImGui::TextWrapped("%s", name.c_str());

        ImGui::NextColumn();

        ImGui::PopID();
    }

    ImGui::Columns(1);

    ImGui::End();
}
