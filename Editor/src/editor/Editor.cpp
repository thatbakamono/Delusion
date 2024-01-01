#include "editor/Editor.hpp"

#include <fstream>
#include <optional>
#include <string>

#include <imgui.h>
#include <nfd.hpp>

#include "delusion/Components.hpp"
#include "delusion/io/FileUtilities.hpp"

Editor::Editor(
    Engine *engine, std::shared_ptr<ScriptEngine> scriptEngine, std::shared_ptr<Texture2D> emptyTexture,
    std::shared_ptr<Texture2D> fileIconTexture, std::shared_ptr<Texture2D> directoryIconTexture,
    std::shared_ptr<Texture2D> playIconTexture, std::shared_ptr<Texture2D> stopIconTexture
)
    : m_engine(engine), m_hierarchyPanel(m_engine), m_viewportPanel(
                                                        *this, m_hierarchyPanel, m_engine, std::move(scriptEngine),
                                                        std::move(playIconTexture), std::move(stopIconTexture)
                                                    ),
      m_assetBrowserPanel(std::move(fileIconTexture), std::move(directoryIconTexture)),
      m_propertiesPanel(*this, m_hierarchyPanel, m_assetManager, std::move(emptyTexture)),
      m_assetManager(engine->assetManager()), m_sceneSerde(m_assetManager) {
    m_engine->setCurrentScene(m_scene);
}

void Editor::onEditorUpdate(std::shared_ptr<Texture2D> &viewportTexture, float deltaTime) {
    if (!m_project.has_value()) {
        onProjectPanel();
    } else {
        auto &project = m_project.value();

        onMenuBar(project);

        m_hierarchyPanel.onUpdate();
        m_viewportPanel.onUpdate(project, viewportTexture, deltaTime);
        m_assetBrowserPanel.onUpdate(project);
        m_propertiesPanel.onUpdate();
    }
}

void Editor::onRuntimeUpdate(float deltaTime) {
    if (m_project.has_value()) {
        if (m_isPlaying) {
            m_engine->currentScene()->forEachEntity([&deltaTime](Entity &entity) {
                if (entity.hasComponent<ScriptComponent>()) {
                    auto &script = entity.getComponent<ScriptComponent>();

                    auto scriptClass = CSharpClass(static_cast<MonoClass *>(script.class_));
                    auto instance = CSharpObject(static_cast<MonoObject *>(script.instance));

                    auto onCreateMethod = scriptClass.getMethod("OnUpdate", 1);

                    float *deltaTimePtr = &deltaTime;

                    mono_runtime_invoke(
                        onCreateMethod, instance.getMonoObject(), reinterpret_cast<void **>(&deltaTimePtr), nullptr
                    );
                }
            });

            m_engine->currentScene()->onUpdate(deltaTime);
        }
    }
}

void Editor::onProjectPanel() {
    ImGui::Begin("Project");

    if (ImGui::Button("Create")) {
        // TODO: Support for non-ascii characters
        NFD::UniquePath path;

        if (NFD::PickFolder(path) == NFD_OKAY) {
            auto projectFilePath = std::filesystem::path(path.get());

            projectFilePath /= "project.delusion";

            std::ofstream(projectFilePath.string());

            m_project = std::make_optional(Project(projectFilePath.parent_path()));

            std::filesystem::create_directory(m_project->assetsDirectoryPath());

            m_assetBrowserPanel.setCurrentDirectory(m_project->assetsDirectoryPath());

            m_fileWatch = std::make_unique<filewatch::FileWatch<std::string>>(
                m_project->assetsDirectoryPath().string(),
                [&](const std::string &path, const filewatch::Event event) { onFileSystemChange(path, event); }
            );
        }
    }

    if (ImGui::Button("Open")) {
        // TODO: Support for non-ascii characters
        NFD::UniquePath path;

        nfdu8filteritem_t filterItem = { "Project file", "delusion" };

        if (NFD::OpenDialog(path, &filterItem, 1) == NFD_OKAY) {
            auto projectFilePath = std::filesystem::path(path.get());
            auto projectPath = projectFilePath.parent_path();

            m_project = std::make_optional(Project(projectFilePath.parent_path()));
            m_assetBrowserPanel.setCurrentDirectory(m_project->assetsDirectoryPath());

            m_assetManager->generateMetadataForAllFiles(m_project->assetsDirectoryPath());
            m_assetManager->loadMappings(m_project->assetsDirectoryPath());

            m_fileWatch = std::make_unique<filewatch::FileWatch<std::string>>(
                m_project->assetsDirectoryPath().string(),
                [&](const std::string &path, const filewatch::Event event) { onFileSystemChange(path, event); }
            );
        }
    }

    ImGui::End();
}

void Editor::onMenuBar(Project &project) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Scene")) {
            if (ImGui::MenuItem("Save as", nullptr, false, m_scene != nullptr)) {
                // TODO: Support for non-ascii characters
                NFD::UniquePath path;

                auto assetsDirectoryString = project.assetsDirectoryPath().string();

                nfdu8filteritem_t filterItem = { "Scene file", "scene" };

                if (NFD::SaveDialog(path, &filterItem, 1, assetsDirectoryString.c_str()) == NFD_OKAY) {
                    std::ofstream stream(path.get());

                    stream << m_sceneSerde.serialize(*m_scene);
                }
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Editor::onFileSystemChange(const std::string &relativePath, const filewatch::Event event) {
    std::filesystem::path path = m_project->assetsDirectoryPath() / relativePath;

    switch (event) {
        case filewatch::Event::added:
            m_assetManager->generateMetadataForFile(path);
            m_assetManager->loadMapping(path);

            break;
        case filewatch::Event::removed:
            m_assetManager->unloadMapping(m_assetManager->getIdByPath(path));
            m_assetManager->deleteMetadataOfFile(path);

            break;
        case filewatch::Event::renamed_old:
            m_fileBeingRenamed = std::make_optional(path);

            break;
        case filewatch::Event::renamed_new:
            m_assetManager->updateMapping(m_assetManager->getIdByPath(m_fileBeingRenamed.value()), path);
            m_assetManager->renameMetadataOfFile(m_fileBeingRenamed.value(), path);

            m_fileBeingRenamed = std::nullopt;

            break;
        default:
            break;
    };
}
