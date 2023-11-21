module;

#include <imgui.h>

export module editor;

export class Editor {
public:
    void update() {
        ImGui::Begin("Hierarchy");

        ImGui::End();

        ImGui::Begin("Viewport");

        ImGui::End();

        ImGui::Begin("Properties");

        ImGui::End();
    }
};
