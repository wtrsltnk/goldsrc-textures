#include "program.h"
#include "texturemanager.h"
#include <imgui.h>
#include <sstream>

Texture *Program::renderGuiTextureBrowser()
{
    Texture *result = nullptr;
    static char search_for[45];

    ImGui::Begin("TextureBrowser", &state.show_texture_browser, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
    {
        ImGui::SetWindowPos(ImVec2(0, 22));
        ImGui::SetWindowSize(ImVec2(state.width > 700 ? 550 : 275, state.height - 22));

        ImGui::BeginChild("filter_textures", ImVec2(ImGui::GetWindowContentRegionWidth(), 30));
        ImGui::Text("Filter");
        ImGui::SameLine();
        if (ImGui::InputText("##search_for", search_for, IM_ARRAYSIZE(search_for), ImGuiInputTextFlags_CharsUppercase))
        {
            state.foundTextures = textures.findTextures(search_for);
        }
        ImGui::EndChild();

        ImGui::BeginChild("stextures", ImVec2(ImGui::GetWindowContentRegionWidth(), state.height - 22 - 40));
        for (auto pair : state.foundTextures)
        {
            if (ImGui::CollapsingHeader(pair.first.c_str()))
            {
                int i = 0;
                for (auto texture : pair.second)
                {
                    if (texture->glId() == 0) texture->upload();

                    auto aspect = glm::min(110.0f / texture->width(), 110.0f / texture->height());

                    if (i++ % (state.width > 700 ? 4 : 2) != 0) ImGui::SameLine();

                    std::stringstream ss;
                    ss << "textureimage" << i;
                    ImGui::BeginGroup();
                    ImGui::PushStyleColor(ImGuiCol_Button, (texture == state.currentTexture ? ImVec4(1, 0, 0, 1) : ImVec4(0, 0, 0, 0)));
                    if (ImGui::ImageButton((void *)texture->glId(), ImVec2(texture->width() * aspect, texture->height() * aspect)))
                    {
                        result = texture;
                    }
                    ImGui::PopStyleColor();
                    ImGui::Text(texture->name().c_str());
                    ImGui::EndGroup();
                }
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();

    return result;
}
