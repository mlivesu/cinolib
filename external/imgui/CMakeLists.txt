cmake_minimum_required(VERSION 3.2)

add_library(imgui
            imgui.cpp
            imgui_demo.cpp
            imgui_draw.cpp
            imgui_tables.cpp
            imgui_widgets.cpp
            imgui_impl_opengl2.cpp
            imgui_impl_glfw.cpp)

option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS    "Build the GLFW test programs"    OFF)
option(GLFW_BUILD_DOCS     "Build the GLFW documentation"    OFF)
option(GLFW_INSTALL        "Generate installation target"    OFF)

add_subdirectory(glfw)
target_link_libraries(imgui PUBLIC glfw)

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_LIST_DIR})
