# Future improvements of Cinolib
Please consider this to be just development notes and not any real planning.

### Things to be fixed:
* make sure per element attributes are propagated to subelements when splitting operators are applied
* make all base destructors virtual (https://codeyarns.com/2016/12/28/always-make-base-class-destructor-as-virtual-in-c/)
* transform all NULL into nullptr
* merge vec2<T> vec3<T> (and colors!) into a unified vec<D,T>. This will make much easier write algorithms that scale across multiple dimensions (e.g. Poisson sampling). For the same reason vertex types should become template parameters for meshes
* provide mechanisms to enable operations between meshes with different template signatures (e.g. export_hexahedra,...)
* add destructors for all classes (https://www.geeksforgeeks.org/virtual-destructor/)
* use a less verbose naming for ANSI text colors
* fix per face winding in midpoint subdivision
* fix marching tets: degenerate triangles will be generated in some cases (see comments inside cpp file)
* fix integral curve tracing (also refactor ray/line/plane intersections)
* gradients on hex-meshes look buggy. Find out why

### Extensions/improvements:
* consider using SSE instructions (https://felix.abecassis.me/2011/09/cpp-getting-started-with-sse/)
* use [HapPly](https://github.com/nmwsharp/happly) for .ply IO operations
* consider moving to NanoGUI for the visual part (https://github.com/mitsuba-renderer/nanogui)
* add line queries to Octree 
* add sharp crease detection to action list in volume mesh control panel
* consider adding a BVH with SAH policy for efficient NN and Ray intersection queries (see http://www.sci.utah.edu/~wald/Publications/2007/ParallelBVHBuild/fastbuild.pdf for theory and https://github.com/wjakob/instant-meshes/blob/master/src/bvh.h for a great implementation)
* consider moving to C++17 to exploit parallel STL functionalities (https://www.bfilipek.com/2018/11/parallel-alg-perf.html)
* add efficient intersection tests for triangle-triangle and segment-segment (https://github.com/gaoxifeng/robust_hex_dominant_meshing/blob/master/src/tri_tri_intersection.h)
* transform all std::cerr into std::cout << ANSI_fg_color_red << 
* add non-manifoldness checks for vertices in srf and vol meshes
* adjust examples #1-#6 such that will read multiple meshes from command line input 
* add reader/writer for .MSH files
* add a "soup" flag to meshes (i.e., no connectivity will be computed)
* add Lagrange multipliers to linear solvers
* add copy constructors for meshes
* add rosy field
* add constructors to create basic meshes (regular 2D/3D lattices,...)
* add converter from hex to tetmesh
* add convenient wraps to do back-substitution directly into linear_solvers.h
* add matrix class (i.e. mat<real,size>, with operators and constructors from quaternions, rotations, identity, diagonal, ecc)
* add support to read/write per element labels in OFF and HEDRA
* make sure field_serialize is lossless in terms of numeric precision
* add inverse and transpose operators for 2x2 and 3x3 matrices
* remove headers from serialized vector and scalar fields (it’s far more general)
* update skeleton data structure (and make relative control panel)
* Add cylinder and spheres list (with colors, size and so forth) in the render list used for meshes, so that there will be only on unified rendering access point
* Refactor PointInsideMeshCache, it should return ALL the elements containing the
  query point and let the application choose the best one
* move from vec2d and vec3d in vec<real,size> (DOABLE?)
* add a system of binary flags to be used by algorithms internal to cinolib
* SlicedObj should not be a trimesh. Its drawable counterpart should!
* vec and Color classes should have similar interfaces
* Add cotan laplacian  normalization
  (ref => https://www.ceremade.dauphine.fr/~peyre/teaching/manifold/tp4.html)
* Improve on mesh rendering (shaders, VBOs (https://www.khronos.org/opengl/wiki/VBO_-_just_examples))
* Use robust geometric computations (volumes, dihedral angles ecc.) 
  (ref. => Lecture Notes on Geometric Robustness di Jonathan Richard Shewchuk)
* add 2D medial axis computation facilities using qgarlib
* ARAP parameterization and shape deformation
* linear blend skinning and dual quaternions

### Documentation:
* create a mirror repo for eigen (and kill that annoying warning)
* add a description of all the non standard files used in cinolib (.hedra, scalar and vector fields, sharp features)

