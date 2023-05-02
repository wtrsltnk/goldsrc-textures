cmake_minimum_required(VERSION 3.7)

include(cmake/CPM.cmake)

project(goldsrc-textures)

CPMAddPackage(
    NAME glm
    GITHUB_REPOSITORY g-truc/glm
    GIT_TAG 0.9.9.7
)

CPMAddPackage(
    NAME glfw
    GITHUB_REPOSITORY "glfw/glfw"
    GIT_TAG 3.3.8
    GIT_SHALLOW ON
    OPTIONS
        "BUILD_SHARED_LIBS Off"
        "GLFW_BUILD_EXAMPLES Off"
        "GLFW_BUILD_TESTS Off"
        "GLFW_BUILD_DOCS Off"
        "GLFW_INSTALL Off"
)

CPMAddPackage(
    NAME imgui
    GIT_TAG docking
    GITHUB_REPOSITORY ocornut/imgui
    DOWNLOAD_ONLY True
)

if (imgui_ADDED)
    message(STATUS "Adding IMGui: ${imgui_SOURCE_DIR}")
    add_library(imgui STATIC
        "${imgui_SOURCE_DIR}/imgui.cpp"
        "${imgui_SOURCE_DIR}/imgui_demo.cpp"
        "${imgui_SOURCE_DIR}/imgui_draw.cpp"
        "${imgui_SOURCE_DIR}/imgui_tables.cpp"
        "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    )

    target_include_directories(imgui
        PUBLIC
            "include"
            "${imgui_SOURCE_DIR}/"
    )

    target_link_libraries(imgui
        PRIVATE
            glfw
    )

    target_compile_options(imgui
        PUBLIC
            -DIMGUI_IMPL_OPENGL_LOADER_GLAD
    )
else()
    message(FATAL_ERROR "IMGUI not found")
endif()

find_package(OpenGL REQUIRED)

add_executable(goldsrc-textures
    src/texturemanager.cpp
    src/texturemanager.h
    src/texturepackage.cpp
    src/texturepackage.h
    src/texture.cpp
    src/texture.h
    src/wadfile.cpp
    src/wadfile.h
    src/wadtexture.cpp
    src/wadtexture.h
    src/bspfile.cpp
    src/bspfile.h
    src/bsptexture.cpp
    src/bsptexture.h
    src/main.cpp
    src/glad.c
    src/program.cpp
    src/program-about.cpp
    src/program-menu.cpp
    src/program-texturebrowser.cpp
    src/program.h
    src/noc_file_dialog.cpp
    include/stb_image_write.h
    include/noc_file_dialog.h
    resource.rc
)

target_include_directories(goldsrc-textures
    PRIVATE include
    PRIVATE lib/system.io/include
)

target_compile_features(goldsrc-textures
    PRIVATE cxx_auto_type
    PRIVATE cxx_nullptr
    PRIVATE cxx_lambdas
    PRIVATE cxx_thread_local
)


target_link_libraries(goldsrc-textures
    glm
    glfw
    imgui
)
