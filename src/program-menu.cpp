#include "program.h"
#include <fstream>
#include <imgui.h>
#include <noc_file_dialog.h>
#include <streambuf>

void Program::renderGuiMenu()
{
    bool runAbout = false;
    bool runMessage = false;
    static std::string message = "";

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4());
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open", "CTRL+O"))
                {
                    if (!openAsset())
                    {
                        runMessage = true;
                        message = "Opening file failed";
                    }
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Quit")) quitApp();
                ImGui::EndMenu(); // File
            }
            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("About Hatchet")) runAbout = true;
                ImGui::EndMenu(); // Help
            }
            ImGui::EndMainMenuBar();
        }
    }
    ImGui::PopStyleColor();

    if (runAbout)
    {
        ImGui::OpenPopup("About");
    }
    renderGuiAbout();

    if (runMessage)
    {
        ImGui::OpenPopup("Message");
    }
    if (ImGui::BeginPopupModal("Message", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text(message.c_str());
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

bool Program::openAsset()
{
    // In time, we also should add the following two file types:
    // * Half-Life BSP file (*.bsp)\0*.bsp\0
    // * Half-Life MDL file (*.mdl)\0*.mdl\0
    const char *filer = "Half-Life WAD file (*.wad)\0*.wad\0Half-Life executable (hl.exe)\0hl.exe\0Half-Life BSP file (*.bsp)\0*.bsp\0";
    const char *file = noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, filer, nullptr, nullptr);
    if (file != nullptr)
    {
        auto fileInfo = System::IO::FileInfo(file);
        if (fileInfo.Name() == "hl.exe")
        {
            populateTextureManagerFromOptions(file);

            return true;
        }
        else if (fileInfo.Extension() == ".wad" || fileInfo.Extension() == ".WAD")
        {
            textures.addTexturesFromWadFile(fileInfo.FullName());

            state.foundTextures = textures.findTextures("");

            return true;
        }
//        else if (fileInfo.Extension() == ".bsp" || fileInfo.Extension() == ".BSP")
//        {
//        }
//        else if (fileInfo.Extension() == ".mdl" || fileInfo.Extension() == ".mdl")
//        {
//        }
    }

    return false;
}

void Program::quitApp()
{
    _running = false;
}
