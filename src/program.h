#ifndef PROGRAM_H
#define PROGRAM_H

#include "texturemanager.h"

#include <GL/glextl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <system.io.fileinfo.h>

struct app_state
{
    bool show_texture_browser = true;
    bool show_texture_view = true;
    float width = 200.0f;
    float height = 300.0f;
    int mousex = 0;
    int mousey = 0;
    bool shiftPressed = false;
    bool ctrlPressed = false;
    std::map<std::string, std::vector<Texture *>> foundTextures;
    Texture *currentTexture = nullptr;
};

class Program
{
private:
    GLFWwindow *_window;

    app_state state;
    TextureManager textures;

    void populateTextureManagerFromOptions(std::string const &hlExecutablePath);
    void addTexturesFromPath(System::IO::DirectoryInfo const &path);

    void renderGuiMenu();
    void renderGuiAbout();
    Texture *renderGuiTextureBrowser();
    void renderGuiTextureView(Texture *texture);

    bool openAsset();
    void quitApp();

public:
    static void KeyActionCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void ResizeCallback(GLFWwindow *window, int width, int height);
    static void CursorPosCallback(GLFWwindow *window, double x, double y);
    static void ScrollCallback(GLFWwindow *window, double x, double y);
    static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

protected:
    void onKeyAction(int key, int scancode, int action, int mods);
    void onResize(int width, int height);
    void onMouseMove(int x, int y);
    void onMouseButton(int button, int action, int mods);
    void onScroll(int x, int y);

public:
    Program(GLFWwindow *window);
    virtual ~Program();

    bool SetUp();
    void Render();
    void CleanUp();

    bool _running;
};

#endif // PROGRAM_H
