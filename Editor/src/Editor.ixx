module;

#include <imgui.h>

#include <webgpu.h>

export module editor;

export class Editor {
public:
    void update(WGPUTextureView viewportTextureView) {


        ImGui::Begin("Hierarchy");

        ImGui::End();

        {
            ImGui::Begin("Viewport");

            ImVec2 availableSpace = ImGui::GetContentRegionAvail();

            ImGui::Image(viewportTextureView, availableSpace);

            ImGui::End();
        }

        ImGui::Begin("Properties");

        ImGui::End();
    }
};
