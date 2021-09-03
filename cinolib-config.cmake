cmake_minimum_required(VERSION 3.2)

# https://cmake.org/cmake/help/git-stage/module/FetchContent.html
include(FetchContent)

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
# BASIC HEADER ONLY LIBRARY :::::::::::::::::::::::::::::::::::::::::::::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#interface-libraries
add_library(cinolib INTERFACE)
target_include_directories(cinolib INTERFACE $<BUILD_INTERFACE:${cinolib_DIR}/include>)
target_include_directories(cinolib INTERFACE $<BUILD_INTERFACE:${cinolib_DIR}/external/eigen>)

# https://cliutils.gitlab.io/modern-cmake/chapters/features/cpp11.html
target_compile_features(cinolib INTERFACE cxx_std_11)
set_target_properties(cinolib PROPERTIES CXX_EXTENSIONS OFF)

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
# OPTIONAL MODULES ::::::::::::::::::::::::::::::::::::::::::::::::::::::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

option(CINOLIB_USES_OPENGL_GLFW_IMGUI "Use OpenGL, GLFW and ImGui" OFF)
option(CINOLIB_USES_TETGEN            "Use Tetgen"                 OFF)
option(CINOLIB_USES_TRIANGLE          "Use Triangle"               OFF)
option(CINOLIB_USES_EXACT_PREDICATES  "Use Exact Predicates"       OFF)
option(CINOLIB_USES_GRAPH_CUT         "Use Graph Cut"              OFF)
option(CINOLIB_USES_BOOST             "Use Boost"                  OFF)
option(CINOLIB_USES_VTK               "Use VTK"                    OFF)

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

if(CINOLIB_USES_OPENGL_GLFW_IMGUI)
    message("CINOLIB OPTIONAL MODULES: OpenGL, GLFW, ImGui")
    find_package(OpenGL REQUIRED)
    target_link_libraries(cinolib INTERFACE OpenGL::GL)
    add_subdirectory(${cinolib_DIR}/external/imgui imgui)
    target_link_libraries(cinolib INTERFACE imgui)
    target_compile_definitions(cinolib INTERFACE CINOLIB_USES_OPENGL)
endif()

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

if(CINOLIB_USES_TETGEN)
    message("CINOLIB OPTIONAL MODULE: Tetgen")
    FetchContent_Declare(tetgen GIT_REPOSITORY "https://github.com/cinolib-dev-team/tetgen.git")
    FetchContent_MakeAvailable(tetgen)
    target_compile_definitions(cinolib INTERFACE CINOLIB_USES_TETGEN)
    target_compile_definitions(cinolib INTERFACE TETLIBRARY)
    target_include_directories(cinolib INTERFACE ${tetgen_SOURCE_DIR})
    target_link_libraries(cinolib INTERFACE tetgen)
endif()

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

if(CINOLIB_USES_TRIANGLE)
    message("CINOLIB OPTIONAL MODULE: Triangle")
    FetchContent_Declare(triangle GIT_REPOSITORY "https://github.com/cinolib-dev-team/triangle.git")
    FetchContent_MakeAvailable(triangle)
    target_compile_definitions(cinolib INTERFACE CINOLIB_USES_TRIANGLE)
    target_include_directories(cinolib INTERFACE ${triangle_SOURCE_DIR})
    target_link_libraries(cinolib INTERFACE triangle)
endif()

##::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

if(CINOLIB_USES_EXACT_PREDICATES)
    message("CINOLIB OPTIONAL MODULE: Exact Predicates")
    add_subdirectory(${cinolib_DIR}/external/predicates predicates)
    target_link_libraries(cinolib INTERFACE predicates)
    target_compile_definitions(cinolib INTERFACE CINOLIB_USES_EXACT_PREDICATES)
endif()

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

if(CINOLIB_USES_GRAPH_CUT)
    message("CINOLIB OPTIONAL MODULE: Graph Cuts")
    FetchContent_Declare(graph_cuts GIT_REPOSITORY "https://github.com/cinolib-dev-team/GraphCuts.git")
    FetchContent_MakeAvailable(graph_cuts)
    target_compile_definitions(cinolib INTERFACE CINOLIB_USES_GRAPH_CUT)
    target_link_libraries(cinolib INTERFACE graph_cuts)
    target_include_directories(cinolib INTERFACE ${graph_cuts_SOURCE_DIR})
endif()

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

if(CINOLIB_USES_BOOST)
    message("CINOLIB OPTIONAL MODULE: Boost")
    find_package(Boost)
    if(Boost_FOUND)
        target_include_directories(cinolib INTERFACE ${boost_INCLUDE_DIRS})
        target_compile_definitions(cinolib INTERFACE CINOLIB_USES_BOOST)
    else()
        message("Could not find Boost!")
    endif()
endif()

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

if(CINOLIB_USES_VTK)
    message("CINOLIB OPTIONAL MODULE: VTK")
    find_package(VTK)
    if(VTK_FOUND)
        include(${VTK_USE_FILE})
        target_link_libraries(cinolib INTERFACE ${VTK_LIBRARIES})
        target_compile_definitions(cinolib INTERFACE CINOLIB_USES_VTK)
    else()
        message("Could not find VTK!")
    endif()
endif()

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
