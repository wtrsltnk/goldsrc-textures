#include "program.h"

#include <GL/glextl.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <imgui_internal.h>

#define SYSTEM_IO_FILEINFO_IMPLEMENTATION
#include <system.io.fileinfo.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

Program::Program(GLFWwindow *window)
    : _window(window), _running(true)
{
    glfwSetWindowUserPointer(this->_window, static_cast<void *>(this));
}

Program::~Program()
{
    glfwSetWindowUserPointer(this->_window, nullptr);
}

void Program::addTexturesFromPath(System::IO::DirectoryInfo const &path)
{
    for (auto file : path.GetFiles())
    {
        auto fileInfo = System::IO::FileInfo(System::IO::Path::Combine(path.FullName(), file));
        if (fileInfo.Extension() == ".wad")
        {
            textures.addTexturesFromWadFile(fileInfo.FullName());
        }
    }
}

void Program::populateTextureManagerFromOptions(std::string const &hlExecutablePath)
{
    auto hl = System::IO::FileInfo(hlExecutablePath);
    auto hlPath = hl.Directory();
    for (auto sub : hlPath.GetDirectories())
    {
        auto subPath = System::IO::DirectoryInfo(System::IO::Path::Combine(hlPath.FullName(), sub));

        addTexturesFromPath(subPath);
    }

    state.foundTextures = textures.findTextures("");
}

bool Program::SetUp()
{
    modal.show = false;
    modal.message = "";

    ImGuiIO &io = ImGui::GetIO();
    if (io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 16.0f) == nullptr)
    {
        return false;
    }

    ImFontConfig config;
    config.MergeMode = true;

    static const ImWchar icons_ranges_fontawesome[] = {0xf000, 0xf3ff, 0};
    if (io.Fonts->AddFontFromFileTTF("fontawesome-webfont.ttf", 22.0f, &config, icons_ranges_fontawesome) == nullptr)
    {
        return false;
    }

    static const ImWchar icons_ranges_googleicon[] = {0xe000, 0xeb4c, 0};
    if (io.Fonts->AddFontFromFileTTF("MaterialIcons-Regular.ttf", 24.0f, &config, icons_ranges_googleicon) == nullptr)
    {
        return false;
    }

    return true;
}

void Program::onResize(int width, int height)
{
    state.width = width;
    state.height = height;
}

void Program::renderGuiTextureView(Texture *texture)
{
    auto browserWidth = state.width > 700 ? 550 : 275;
    auto textureWidth = state.width  - browserWidth;
    if (texture != nullptr)
    {
        ImGui::Begin("TextureView", &state.show_texture_view, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::SetWindowPos(ImVec2(browserWidth, 22));
            ImGui::SetWindowSize(ImVec2(textureWidth, state.height - 22));

            std::stringstream ss;
            ss << texture->name() << " (" << texture->width() << "x" << texture->height() << ")";
            ImGui::Text(ss.str().c_str());
            ImGui::SameLine(ImGui::GetWindowWidth()-70);
            if (ImGui::Button("Export", ImVec2(70, 30)))
            {
                if (exportTexture(texture))
                {
                    ImGui::OpenPopup("Messagebox");
                }
            }
            bool open = true;
            if (ImGui::BeginPopupModal("Messagebox", &open))
            {
                ImGui::Text("The texture is exported");
                if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            auto aspect = glm::min((textureWidth) / texture->width(), (state.height - 100) / texture->height());
            ImGui::Image((void *)texture->glId(), ImVec2(texture->width() * aspect, texture->height() * aspect));
        }
        ImGui::End();
    }
}

void Program::Render()
{
    glViewport(0, 0, state.width, state.height);
    glClearColor(114 / 255.0f, 144 / 255.0f, 154 / 255.0f, 255 / 255.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ImGui_ImplGlfwGL3_NewFrame();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 1.0f);
    {
        renderGuiMenu();

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.20f, 0.20f, 0.47f, 0.60f));
        {
            auto tex = renderGuiTextureBrowser();
            if (tex != nullptr)
            {
                state.currentTexture = tex;
            }
            renderGuiTextureView(state.currentTexture);
        }
        ImGui::PopStyleColor();
    }
    ImGui::PopStyleVar();

    ImGui::Render();
}

void Program::onKeyAction(int key, int scancode, int action, int mods)
{
    state.shiftPressed = (mods & GLFW_MOD_SHIFT);
    state.ctrlPressed = (mods & GLFW_MOD_CONTROL);
}

void Program::onMouseMove(int x, int y)
{
    state.mousex = x;
    state.mousey = y;
}

void Program::onMouseButton(int button, int action, int mods)
{
}

void Program::onScroll(int x, int y)
{
    if (state.shiftPressed)
    {
    }
    else if (state.ctrlPressed)
    {
    }
    else
    {
    }
}

void Program::CleanUp() {}

void Program::KeyActionCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);

    auto app = static_cast<Program *>(glfwGetWindowUserPointer(window));

    if (app != nullptr)
        app->onKeyAction(key, scancode, action, mods);
}

void Program::CursorPosCallback(GLFWwindow *window, double x, double y)
{
    auto app = static_cast<Program *>(glfwGetWindowUserPointer(window));

    if (app != nullptr)
        app->onMouseMove(int(x), int(y));
}

void Program::ScrollCallback(GLFWwindow *window, double x, double y)
{
    ImGui_ImplGlfwGL3_ScrollCallback(window, x, y);

    auto app = static_cast<Program *>(glfwGetWindowUserPointer(window));

    if (app != nullptr)
        app->onScroll(int(x), int(y));
}

void Program::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);

    auto app = static_cast<Program *>(glfwGetWindowUserPointer(window));

    if (app != nullptr)
        app->onMouseButton(button, action, mods);
}

void Program::ResizeCallback(GLFWwindow *window, int width, int height)
{
    auto app = static_cast<Program *>(glfwGetWindowUserPointer(window));

    if (app != nullptr)
        app->onResize(width, height);
}
