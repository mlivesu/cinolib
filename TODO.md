# Future improvements of Cinolib
Please consider this to be just development notes and not any real planning.

### Papers I'd like to implement:
* Properties of Laplace Operators for Tetrahedral Meshes (SGP2020)
* A Laplacian for Nonmanifold Triangle Meshes (SGP2020)
* Polygon Laplacian Made Simple (EG2020)

### Tips and Tricks to test/implement
* https://github.com/Ten0/updatable_priority_queue (for Dijkstra)
* https://zeux.io/2010/10/17/aabb-from-obb-with-component-wise-abs/

### Things to be fixed:
* Dijkstra is faster with std::priority_queue than with std::set (double check this)
* use enum classes instead of enums for strong typing and easier code/parameter handling
* in DrawableSegmentSoup, edge rendering is orientation dependend when cheap mode is not active (cylinders are defined as points + dir!)
* find ways to speedup updateGL(). For big meshes it's overly slow...
* transform all NULL into nullptr
* merge vec2<T> vec3<T> (and colors!) into a unified vec<D,T>. This will make much easier write algorithms that scale across multiple dimensions (e.g. Poisson sampling). For the same reason vertex types should become template parameters for meshes
* provide mechanisms to enable operations between meshes with different template signatures (e.g. export_hexahedra,...)
* use a less verbose naming for ANSI text colors
* fix per face winding in midpoint subdivision
* fix marching tets: degenerate triangles will be generated in some cases (see comments inside cpp file)
* fix integral curve tracing (also refactor ray/line/plane intersections)
* gradients on hex-meshes look buggy. Find out why

### Extensions/improvements:
* create a struct IOData that contains vectors for all input mesh elements (verts, polys, textures, normals, labels, colors, ecc), and use it for any IO operation in the lib
* remove vecs of vecs for mesh polygons and polyhedra. use serialized elements and two separated vectors to index them
* add line color for 2D checkerboard maps
* allow to select clamping or repeation for 1D texture
* implement convertion operators between surface meshes and volume meshes (see http://www.cplusplus.com/doc/tutorial/typecasting/)
* use parallel for to handle all-vs-all tests in octree leaves
* splitting rule for octree generation should be passed as an external function, so that each one can make the tree he wants to make....
* double check vertex non manifoldness checks, they can probably be made much faster exploiting Euler to check the topology of the link....
* remove vectors of vectors and use serialized indices (with end of list markers, and pointers to base addr for each element). Measure gains before!
* parsers should return vectors of doubles, not vec3ds
* it would be better to have vectors of doubles inside the meshes too, and have a vec3d "view" on data to do math
* use PARALLEL_FOR as much as possible inside the library to boost performances
* consider computing element quality and normals (at least for faces) on the fly, without precomputing and storing them
* try to adhere more to data flow/functional programming principles, keeping as few classes as possible with only access to inner data, moving methods that do data processing outside the class
* consider using SSE instructions (http://www.cs.uu.nl/docs/vakken/magr/2017-2018/files/SIMD%20Tutorial.pdf)
* use [HapPly](https://github.com/nmwsharp/happly) for .ply IO operations
* add line queries to Octree
* consider adding a BVH with SAH policy for efficient NN and Ray intersection queries (see http://www.sci.utah.edu/~wald/Publications/2007/ParallelBVHBuild/fastbuild.pdf for theory and https://github.com/wjakob/instant-meshes/blob/master/src/bvh.h for a great implementation)
* consider moving to C++17 to exploit parallel STL functionalities (https://www.bfilipek.com/2018/11/parallel-alg-perf.html)
* adjust examples #1-#6 such that will read multiple meshes from command line input
* add reader/writer for .MSH files
* add a "soup" flag to meshes (i.e., no connectivity will be computed)
* add Lagrange multipliers to linear solvers
* add copy constructors for meshes
* add rosy field
* add constructors to create basic meshes (regular 2D/3D lattices,...)
* add convenient wraps to do back-substitution directly into linear_solvers.h
* add support to read/write per element labels in OFF and HEDRA
* make sure field_serialize is lossless in terms of numeric precision
* remove headers from serialized vector and scalar fields (it’s far more general)
* update skeleton data structure (and make relative control panel)
* SlicedObj should not be a trimesh. Its drawable counterpart should!
* vec and Color classes should have similar interfaces
* Add cotan laplacian  normalization
  (ref => https://www.ceremade.dauphine.fr/~peyre/teaching/manifold/tp4.html)
* Improve on mesh rendering (shaders, VBOs (https://www.khronos.org/opengl/wiki/VBO_-_just_examples))
* Use robust geometric computations (volumes, dihedral angles ecc.)
  (ref. => Lecture Notes on Geometric Robustness di Jonathan Richard Shewchuk)
* add 2D medial axis computation facilities using qgarlib
* linear blend skinning and dual quaternions

### Documentation:
* make animated GIFs to illustrate examples
* add a description of all the non standard files used in cinolib (.hedra, scalar and vector fields, sharp features)
