# Examples
This folder contains a number of sample programs that illustrate the core functionalities of CinoLib.

### How to compile the example projects
To compile the examples you must download CinoLib on your machine and install [Qt](https://www.qt.io) for the GUI. You can compile all the examples together by considering the file `build_all_examples.pro`, or compile a specific example by considering its project file (.pro). Project files can be compiled either with [QtCreator](https://www.qt.io/qt-features-libraries-apis-tools-and-ide/) or by opening a terminal in the same folder and typing
```
qmake .
make -j4
```

### External dependencies
Some of the projects depend from external libraries which are wrapped in CinoLib (e.g. [Triangle](https://www.cs.cmu.edu/~quake/triangle.html), [Tetgen](http://wias-berlin.de/software/index.jsp?id=TetGen&lang=1) or [VTK](https://www.vtk.org)). These libraries should be installed separately, and the project files updated with correct paths for compiler and linker. Projects with external dependencies are marked in `build_all_examples.pro`. If you are not interested in these examples, you can safely comment them by adding `#` at the beginning of the corresponding lines in the project file, and they will be automatically ignored by qmake.

# List of Examples
Here is a list of the sample programs available in CinoLib.

#### 01 - Load and render a triangle mesh
<p align="left"><img src="snapshots/01_base_app_trimesh.png" width="500"></p>

#### 02 - Load and render a quadrilateral mesh
<p align="left"><img src="snapshots/02_base_app_quadmesh.png" width="500"></p>

#### 03 - Load and render a general polygonal mesh
<p align="left"><img src="snapshots/03_base_app_polygonmesh.png" width="500"></p>

#### 04 - Load and render a tetrahedral mesh
<p align="left"><img src="snapshots/04_base_app_tetmesh.png" width="500"></p>

#### 05 - Load and render a hexahedral mesh
<p align="left"><img src="snapshots/05_base_app_hexmesh.png" width="500"></p>

#### 06 - Load and render a general polyhedral mesh
<p align="left"><img src="snapshots/06_base_app_polyhedralmesh.png" width="500"></p>

#### 07 - Load and render a textured object in OBJ format
<p align="left"><img src="snapshots/07_textured_OBJs.png" width="500"></p>

#### 08 - Do vertex picking for interactive applications
<p align="left"><img src="snapshots/08_picking.png" width="500"></p>

#### 09 - Compute polyharmonic functions on discrete surfaces
<p align="left"><img src="snapshots/09_polyharmonic_functions_srf.png" width="500"></p>

#### 10 - Compute polyharmonic functions on discrete volumes
<p align="left"><img src="snapshots/10_polyharmonic_functions_vol.png" width="500"></p>

#### 11 - Map a closed object to a sphere using conformalized mean curvature flow
<p align="left"><img src="snapshots/11_map_to_sphere.png" width="500"></p>

#### 12 - Generate triangular and polygonal tessellations of 2D domains
<p align="left"><img src="snapshots/12_polygon_mesh_generation.png" width="500"></p>

#### 13 - Generate tetrahedral and polyhedral tessellations of 3D domains
<p align="left"><img src="snapshots/13_polyhedral_mesh_generation.png" width="500"></p>

#### 14 - Compute geodesic distances using the heat flow method
<p align="left"><img src="snapshots/14_heat_based_geodesics.png" width="500"></p>

#### 15 - Compute kernels, incircles and circumcircles of 2D polygons
<p align="left"><img src="snapshots/15_polygon_measures.png" width="500"></p>

#### 16 - Use spherical Fibonacci to evenly sample the unit sphere
<p align="left"><img src="snapshots/16_sphere_sampling.png" width="500"></p>

#### 17 - Extract ISO contours from surface meshes
<p align="left"><img src="snapshots/17_iso_contours.png" width="500"></p>

#### 18 - Extract ISO surfaces using marching cubes for tetrahedral meshes
<p align="left"><img src="snapshots/18_iso_surfaces.png" width="500"></p>

#### 19 - Compute discrete harmonic maps
<p align="left"><img src="snapshots/19_harmonic_map.png" width="500"></p>

#### 20 - Compute coarse quad layouts
<p align="left"><img src="snapshots/20_coarse_quad_layouts.png" width="500"></p>

#### 21 - Compute coarse hex layouts
<p align="left"><img src="snapshots/21_coarse_hex_layouts.png" width="500"></p>

#### 22 - Remesh, preserving sharp creases or color patches
<p align="left"><img src="snapshots/22_remeshing.png" width="500"></p>

#### 23 - Find, adjust, and export sharp creases from a mesh
<p align="left"><img src="snapshots/23_sharp_creases.png" width="500"></p>

#### 24 - Read and visualize a sliced object for 3D printing
<p align="left"><img src="snapshots/24_sliced_obj.png" width="500"></p>

# Upcoming examples
Maintaining a library alone is very time consuming, and the amount of time I can spend on CinoLib is limited. I do my best to keep the number of examples constantly growing. I am currently working on various code samples that showcase other core functionalities of CinoLib. All (but not only) these topics will be covered:

* Copy/paste points of view from the GL canvas for accurate visual comparisons and fast snapshot reproduction
* Customize data structures with user defined mesh/vertex/edge/polygon/polyhedron attributes
* Navigate application history exploiting Cinolib undo/redo facilities
* Generate, process and visualize scalar fields
* Generate, process and visualize vector fields
* Surface and volumetric format converters
* Topology editing operators for triangle meshes
* Topology editing operators for tetrahedral meshes
* Topology editing operators for general polygonal meshes
* Topology editing operators for general polyhedral meshes
* Subdivision schemas for hexahedral meshes
* Parameterization based hexahedral remeshing
* Evaluation of mapping distortions (angle/areas distortion popular metrics)
* Solve multi-labeling problems on surfaces and volumes using the [GraphCuts](http://vision.csd.uwo.ca/code/) wrapper
* Integral curve computation (and tessellation)
