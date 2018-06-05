TEMPLATE = subdirs

# To compile the projects either open this file with QtCreator or launch a terminal
# in this folder and type:
#
#  qmake .
#  make -j6
#
# Notice that some of the projects have external dependecies (see comments below).
# To be able to compile these projects you need to install the external libraries
# from which they depend, and edit the corresponding project files (.pro) providing
# paths for compiler and linker. If you are not interested in these projects, you
# can easily exclude them from the build by adding the character # at the beginning
# of the "SUBDIR +=" lines that include them. For optional dependencies, you can
# also decide not to install the missing library. In this case you should open the
# project file (.pro) and comment the lines where the optional library is invoked.
# Then, you should be able to compile.

SUBDIRS += 01_base_app_trimesh
SUBDIRS += 02_base_app_quadmesh
SUBDIRS += 03_base_app_polygonmesh
SUBDIRS += 04_base_app_tetmesh           # OPTIONALLY requires VTK (https://www.vtk.org/)
SUBDIRS += 05_base_app_hexmesh           # OPTIONALLY requires VTK (https://www.vtk.org/)
SUBDIRS += 06_base_app_polyhedralmesh
SUBDIRS += 07_textured_OBJs
SUBDIRS += 08_picking
SUBDIRS += 09_polyharmonic_functions_srf
SUBDIRS += 10_polyharmonic_functions_vol # OPTIONALLY requires VTK (https://www.vtk.org/)
SUBDIRS += 11_map_to_sphere
SUBDIRS += 12_polygon_mesh_generation    # requires Triangle (https://www.cs.cmu.edu/%7Equake/triangle.html)
SUBDIRS += 13_polyhedral_mesh_generation # requires Tetgen (http://wias-berlin.de/software/index.jsp?id=TetGen&lang=1)
SUBDIRS += 14_heat_based_geodesics
SUBDIRS += 15_polygon_measures           # requires Boost (http://www.boost.org)
SUBDIRS += 16_sphere_sampling
SUBDIRS += 17_iso_contours
SUBDIRS += 18_iso_surfaces               # OPTIONALLY requires VTK (https://www.vtk.org/)
SUBDIRS += 19_harmonic_map
SUBDIRS += 20_coarse_quad_layouts
SUBDIRS += 21_coarse_hex_layouts        # OPTIONALLY requires VTK (https://www.vtk.org/)
SUBDIRS += 22_remesher
SUBDIRS += 23_sharp_creases
SUBDIRS += 24_sliced_obj                # requires Boost (http://www.boost.org) and Triangle (https://www.cs.cmu.edu/%7Equake/triangle.html)

