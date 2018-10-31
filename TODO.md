# Future improvements of Cinolib
Please consider this to be just development notes and not any real planning.

### Things to be fixed:
* fix per face winding in midpoint subdivision
* fix marching tets: degenerate triangles will be generated in some cases (see comments inside cpp file)
* fix integral curve tracing (also refactor ray/line/plane intersections)
* gradients on hex-meshes look buggy. Find out why

### Extensions/improvements:
* prevent averaging of normals on sharp creases in smooth shading
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
* add verdict metrics for tris/quads/tets/hexa
* put edge flip and similar operators on separate files
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
* object space ambient occlusion 
* ARAP parameterization and shape deformation
* Least Squares Conformal Maps
* linear blend skinning and dual quaternions

### Documentation:
* create a mirror repo for eigen (and kill that annoying warning)
* create a mirror repo for graph cuts and Shewcuck's predicates
* add a description of all the non standard files used in cinolib (.hedra, scalar and vector fields, sharp features)

