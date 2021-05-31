/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#ifndef CINO_EARCUT_MAPPING_H
#define CINO_EARCUT_MAPPING_H

#include <cinolib/geometry/vec_mat.h>
#include <cinolib/symbols.h>

namespace cinolib
{

/* The earcut mapping algorithm generates a provably bijective map between
 * two simple polygons (poly_A and poly_B). Th algorithm puts no restrictions
 * on the geometry of poly_A, whereas poly_B can be either a strictly convex
 * polygon, or a star-shaped polygon with non degenerate (i.e. pointwise) kernel.
 *
 * In case poly_B is strictly convex, the mapping is obtained by simply running
 * the arcut algorithm on poly_A. Due to convexity, the so generated triangles
 * will be valid also for poly_B, generating to meshes with same connectivity,
 * hance a mapping between them.
 *
 * In case poly_B is not strictly convex, the algorith will firstly generate
 * an inner padding layer inside both polygons, triangulating the space in
 * between. In poly_B, the inner layer will sample the perimeter of a disk
 * lying inside the kernel of the polygon. The mapping will then be completed
 * by applying the earcut mapping to the polygon obtained with the inner offsetting.
 *
 * Technical details and ideas for both methods can be found in:
 *
 *     Mapping Surfaces with Earcut
 *     M. Livesu
 *     (submitted)
 *
 *     and
 *
 *     A Mesh Generation Perspective on Robust Mappings
 *     M. Livesu
 *     Smart Tools and Apps for Graphics (STAG, 2020)
 *
 *
 * INPUT DATA:
 *  - circular list of vertice of poly_A
 *  - circular list of vertice of poly_B (ordering consistent with poly_A)
 *  - strictly_convex (true/false)
 *  - disk_center (only if strictly_convex is false)
 *  - disk_radius (only if strictly_convex is false)
 *
 * OUTPUT DATA:
 *  - list of triangles (vector tris, indexing both poly_A and poly_B)
 *  - updated list of vertices poly_A, poly_B (only if strictly_convex was set to false)
 *
 *
 * NOTE: due to some annoying code flaw, even though the method is fully 2D
 * the code inputs 3D data. The z coordinate will be ignored, but the padding
 * code generates a 3D triangle mesh to exploit some of its topological operators,
 * that's why (fake) 3D points are needed. I will eventually fix this...
*/

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    std::vector<vec3d> poly_A; // circular list of vertices for polygon A
    std::vector<vec3d> poly_B; // circular list of vertices for polygon A
    std::vector<uint>  tris;   // serialized triangles (for both A and B)

    bool   strictly_convex = true;     // true if polygon B is strictly convex, false otherwise
    vec3d  disk_center = vec3d(0,0,0); // center of a disk inside polygon B (used when strictly_convex is false)
    float  disk_radius = 0.4;          // radius of a disk inside polygon B (used when strictly_convex is false)
}
EarcutMapping_data;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void earcut_mapping(EarcutMapping_data & data);

}

#ifndef  CINO_STATIC_LIB
#include "earcut_mapping.cpp"
#endif

#endif // CINO_EARCUT_MAPPING_H
