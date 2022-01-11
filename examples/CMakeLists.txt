cmake_minimum_required(VERSION 3.2)

project(examples)

# set cinolib options. projects depending from
# options that are disabled won't be compiled
set(CINOLIB_USES_OPENGL_GLFW_IMGUI ON )
set(CINOLIB_USES_TETGEN            ON )
set(CINOLIB_USES_TRIANGLE          ON )
set(CINOLIB_USES_EXACT_PREDICATES  OFF)
set(CINOLIB_USES_GRAPH_CUT         OFF)
set(CINOLIB_USES_BOOST             ON )
set(CINOLIB_USES_VTK               OFF)

# pass cinolib and the external dependencies to all examples
find_package(cinolib REQUIRED)
link_libraries(cinolib)

# define the path where all the input meshes are available
add_compile_definitions(DATA_PATH="${PROJECT_SOURCE_DIR}/data")

# make a bin folder to host all the executables
make_directory(${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

# suppress GLU deprecated warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

#list of examples
add_subdirectory(01_trimesh_viewer)
add_subdirectory(02_quadmesh_viewer)
add_subdirectory(03_polygonmesh_viewer)
add_subdirectory(04_tetmesh_viewer)
add_subdirectory(05_hexmesh_viewer)
add_subdirectory(06_polyhedralmesh_viewer)
add_subdirectory(07_texture_loading)
add_subdirectory(08_picking)
add_subdirectory(09_polyharmonic_functions_srf)
add_subdirectory(10_polyharmonic_functions_vol)
add_subdirectory(11_sphere_mapping)
if(CINOLIB_USES_TRIANGLE)
#    add_subdirectory(12_polygon_mesh_generation) linking issues (due to name mangling?)
endif()
if(CINOLIB_USES_TETGEN)
    add_subdirectory(13_polyhedral_mesh_generation)
endif()
add_subdirectory(14_heat_geodesics)
if(CINOLIB_USES_BOOST)
    add_subdirectory(15_polygon_measures)
endif()
add_subdirectory(16_sphere_sampling)
add_subdirectory(17_iso_contours)
add_subdirectory(18_iso_surfaces)
add_subdirectory(19_harmonic_map)
add_subdirectory(20_coarse_quad_layouts)
add_subdirectory(21_coarse_hex_layouts)
add_subdirectory(22_remesher)
add_subdirectory(23_sharp_creases)
if(CINOLIB_USES_BOOST AND CINOLIB_USES_TRIANGLE)
#    add_subdirectory(24_sliced_CLI_loader) linking issues (due to name mangling?)
endif()
add_subdirectory(25_surface_painter)
add_subdirectory(26_export_surface)
add_subdirectory(27_import_surface)
if(CINOLIB_USES_TETGEN)
    add_subdirectory(28_tetrahedralize)
endif()
#add_subdirectory(29_ambient_occlusion) It still depends on Qt....
add_subdirectory(30_mesh_format_converter)
add_subdirectory(31_LSCM)
add_subdirectory(32_homotopy_basis)
add_subdirectory(33_edge_collapse)
if(CINOLIB_USES_TETGEN)
    add_subdirectory(34_Hermite_RBF)
endif()
add_subdirectory(35_Poisson_sampling)
add_subdirectory(36_canonical_polygonal_schema)
