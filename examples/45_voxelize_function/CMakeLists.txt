project(voxelize_function)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} cinolib)
