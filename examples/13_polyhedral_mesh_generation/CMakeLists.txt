project(polyhedral_mesh_generation)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} cinolib)
