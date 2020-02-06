#include "program.h"

#include <GL/glextl.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <imgui_internal.h>

#define SYSTEM_IO_FILEINFO_IMPLEMENTATION
#include <system.io.fileinfo.h>
#include <system.io.path.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <thread>
#include <vector>

Program::Program(GLFWwindow *window)
    : _window(window), _statusProgress(-1.0f), _running(true)
{
    state.search_for[0] = '\0';

    glfwSetWindowUserPointer(this->_window, static_cast<void *>(this));
}

Program::~Program()
{
    glfwSetWindowUserPointer(this->_window, nullptr);
}

void Program::updateTextureBrowser()
{
    // this makes sure there are textures shown
    state.foundTextures = textures.findTextures(state.search_for);
}

void Program::SetInputFile(System::IO::FileInfo const &file)
{
    if (file.Exists() && (file.Extension() == ".wad" || file.Extension() == ".WAD"))
    {
        addTexturesFromWad(file);
    }

    if (file.Exists() && (file.Extension() == ".bsp" || file.Extension() == ".BSP"))
    {
        addTexturesFromBsp(file);
    }

    if (file.Exists() && file.Name() == "hl.exe")
    {
        populateTextureManagerFromOptions(file);
    }
}

void Program::addTexturesFromWad(System::IO::FileInfo const &filename)
{
    std::thread t([this, filename]() mutable {
        setStatus(-1.0f, std::string("loading ") + filename.Name() + "...");

        textures.addTexturesFromWadFile(filename.FullName());
        updateTextureBrowser();

        setStatus(-1.0f, "");
    });

    t.detach();
}

void Program::addTexturesFromBsp(System::IO::FileInfo const &filename)
{
    std::thread t([this, filename]() mutable {
        setStatus(-1.0f, std::string("loading ") + filename.Name() + "...");

        textures.addTexturesFromBspFile(filename.FullName());
        updateTextureBrowser();

        setStatus(-1.0f, "");
    });

    t.detach();
}

void Program::addTexturesFromPath(System::IO::DirectoryInfo const &path)
{
    int c = 0;
    auto files = path.GetFiles();
    for (auto file : files)
    {
        auto fileInfo = System::IO::FileInfo(System::IO::Path::Combine(path.FullName(), file));
        if (fileInfo.Extension() == ".wad" || fileInfo.Extension() == ".WAD")
        {
            setStatus(float(c) / float(files.size()), std::string("loading ") + file + "...");

            addTexturesFromWad(fileInfo);
        }
        else if (fileInfo.Extension() == ".bsp" || fileInfo.Extension() == ".BSP")
        {
            setStatus(float(c) / float(files.size()), std::string("loading ") + file + "...");

            addTexturesFromBsp(fileInfo);
        }
    }
}

void Program::populateTextureManagerFromOptions(System::IO::FileInfo const &hl)
{
    std::thread t([this, hl]() mutable {
        auto hlPath = hl.Directory();

        std::vector<System::IO::FileInfo> wadfilesToLoad;
        auto dirs = hlPath.GetDirectories();
        for (auto sub : dirs)
        {
            System::IO::DirectoryInfo path(System::IO::Path::Combine(hlPath.FullName(), sub));
            auto files = path.GetFiles();
            for (auto file : files)
            {
                auto fileInfo = System::IO::FileInfo(System::IO::Path::Combine(path.FullName(), file));
                wadfilesToLoad.push_back(fileInfo);
            }
        }

        int c = 0;
        for (auto fileInfo : wadfilesToLoad)
        {
            if (fileInfo.Extension() == ".wad" || fileInfo.Extension() == ".WAD")
            {
                setStatus(float(c) / float(wadfilesToLoad.size()), std::string("loading ") + fileInfo.Name() + "...");

                textures.addTexturesFromWadFile(fileInfo.FullName());

                updateTextureBrowser();
            }
            c++;
        }

        setStatus(-1.0f, "");
    });

    t.detach();
}

void Program::setStatus(float state, std::string const &message)
{
    _statusProgress = state;
    _statusMessage = message;
}

bool Program::SetUp()
{
    modal.show = false;
    modal.message = "";

    ImGuiIO &io = ImGui::GetIO();
    if (io.Fonts->AddFontFromFileTTF("c:\\windows\\fonts\\verdana.ttf", 16.0f) == nullptr)
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
    auto textureWidth = state.width - browserWidth;
    if (texture != nullptr)
    {
        ImGui::Begin("TextureView", &state.show_texture_view, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::SetWindowPos(ImVec2(browserWidth, menubarHeight));
            ImGui::SetWindowSize(ImVec2(textureWidth, state.height - menubarHeight - statusbarHeight));

            std::stringstream ss;
            ss << texture->name() << " (" << texture->width() << "x" << texture->height() << ")";
            ImGui::Text("%s", ss.str().c_str());
            ImGui::SameLine(ImGui::GetWindowWidth() - 70);
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
    glViewport(0, menubarHeight, state.width, state.height - menubarHeight - statusbarHeight);
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

            ImGui::Begin("statusbar", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
            {
                std::lock_guard<std::mutex> lock(_statusbarMutex);
                ImGui::SetWindowPos(ImVec2(0, state.height - statusbarHeight));
                ImGui::SetWindowSize(ImVec2(state.width, statusbarHeight));

                ImGui::Columns(2);
                ImGui::Text("%s", _statusMessage.c_str());
                ImGui::NextColumn();
                if (_statusProgress >= 0.0f)
                {
                    ImGui::ProgressBar(_statusProgress);
                }
                ImGui::NextColumn();
            }
            ImGui::End();
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
