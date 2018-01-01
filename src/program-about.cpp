#include "program.h"
#include <imgui.h>

void Program::renderGuiAbout()
{
    if (ImGui::BeginPopupModal("About", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("GoldSrc Textures");
        ImGui::Text("A texture viewer for Valve's GoldSrc engine.");
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
