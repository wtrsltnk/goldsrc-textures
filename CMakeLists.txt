cmake_minimum_required(VERSION 3.7)

project(goldsrc-textures)

find_package(GLM REQUIRED)
find_package(OpenGL REQUIRED)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(lib/glfw)

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
    lib/imgui/imgui.cpp
    lib/imgui/imgui_draw.cpp
    src/imgui_impl_glfw_gl3.cpp
    include/imgui_impl_glfw_gl3.h
    src/main.cpp
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
    PRIVATE lib/glfw/include
    PRIVATE lib/imgui
    PRIVATE lib/glm
    PRIVATE lib/system.io/include
    )

target_compile_features(goldsrc-textures
    PRIVATE cxx_auto_type
    PRIVATE cxx_nullptr
    PRIVATE cxx_lambdas
    PRIVATE cxx_thread_local
    )


target_link_libraries(goldsrc-textures
    ${OPENGL_LIBRARIES}
    glfw
    )

ADD_CUSTOM_COMMAND(
    TARGET goldsrc-textures
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ARGS ${CMAKE_SOURCE_DIR}/lib/imgui/extra_fonts/Roboto-Medium.ttf ${CMAKE_BINARY_DIR}/Roboto-Medium.ttf
    COMMAND ${CMAKE_COMMAND} -E copy ARGS ${CMAKE_SOURCE_DIR}/fontawesome-webfont.ttf ${CMAKE_BINARY_DIR}/fontawesome-webfont.ttf
    COMMAND ${CMAKE_COMMAND} -E copy ARGS ${CMAKE_SOURCE_DIR}/MaterialIcons-Regular.ttf ${CMAKE_BINARY_DIR}/MaterialIcons-Regular.ttf
)
