TEMPLATE = subdirs

SUBDIRS += 01_base_app_trimesh
SUBDIRS += 02_base_app_quadmesh
SUBDIRS += 03_base_app_polygonmesh
SUBDIRS += 04_base_app_tetmesh              # requires VTK (https://www.vtk.org/)
SUBDIRS += 05_base_app_hexmesh              # requires VTK (https://www.vtk.org/)
SUBDIRS += 06_base_app_polyhedralmesh
SUBDIRS += 07_textured_OBJs
SUBDIRS += 08_polyharmonic_functions_srf
SUBDIRS += 09_polyharmonic_functions_vol    # requires VTK (https://www.vtk.org/)
SUBDIRS += 10_map_to_sphere
SUBDIRS += 11_polygon_mesh_generation       # requires Triangle (https://www.cs.cmu.edu/%7Equake/triangle.html)
SUBDIRS += 12_polyhedral_mesh_generation    # requires Tetgen (http://wias-berlin.de/software/index.jsp?id=TetGen&lang=1)
SUBDIRS += 13_heat_based_geodesics
SUBDIRS += 14_polygon_measures              # requires Boost (http://www.boost.org)
SUBDIRS += 15_sphere_sampling
SUBDIRS += 16_iso_surfaces
SUBDIRS += 17_harmonic_map
