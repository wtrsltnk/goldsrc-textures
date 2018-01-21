
#include "program.h"

#define GLEXTL_IMPLEMENTATION
#include <GL/glextl.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <thread>

using namespace std;

int main(int argc, char *argv[])
{
    if (glfwInit() == GLFW_FALSE)
    {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1024, 768, "GoldSrc Textures", NULL, NULL);
    if (window == 0)
    {
        glfwTerminate();
        return -1;
    }

    Program app(window);

    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(window, true);

    glfwSetKeyCallback(window, Program::KeyActionCallback);
    glfwSetCursorPosCallback(window, Program::CursorPosCallback);
    glfwSetScrollCallback(window, Program::ScrollCallback);
    glfwSetMouseButtonCallback(window, Program::MouseButtonCallback);

    glfwSetWindowSizeCallback(window, Program::ResizeCallback);
    glfwMakeContextCurrent(window);

    glExtLoadAll((PFNGLGETPROC *)glfwGetProcAddress);

    Program::ResizeCallback(window, 1024, 768);

    if (app.SetUp())
    {
        if (argc > 1)
        {
            std::thread t([argc, argv, &app]() mutable {
                System::IO::FileInfo inputFile(argv[1]);
                if (inputFile.Exists())
                {
                    app.SetInputFile(inputFile);
                }
            });
            t.detach();
        }

        while (glfwWindowShouldClose(window) == 0 && app._running)
        {
            glfwWaitEvents();

            glClear(GL_COLOR_BUFFER_BIT);

            app.Render();

            glfwSwapBuffers(window);
        }
        app.CleanUp();
    }

    glfwTerminate();

    return 0;
}
