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
#ifndef CINO_MESH_ATTRIBUTES_H
#define CINO_MESH_ATTRIBUTES_H

#include <cinolib/geometry/vec3.h>
#include <cinolib/color.h>
#include <string>
#include <bitset>

namespace cinolib
{

/* These are the MINIMAL attributes required by CinoLib.
 * If omitted, the library will not compile!!!!!!!!!!!
 *
 * NOTE: you do not need to specify them, they have been
 * set as default template arguments in any mesh available
 * within the library. Meshes can therefore be declared as
 * follows:
 *
 * Trimesh<>        my_trimesh;
 * Quadmesh<>       my_quadmesh;
 * Polygonmesh<>    my_polygonmesh;
 * Tetmesh<>        my_tetmesh;
 * Hexmesh<>        my_hexmesh;
 * Polyhedralmesh<> my_polyhedralmesh;
 *
 * Otherwise, if more attributes are necessary, you can always
 * extend the current structures, and use explicit templates:
 *
 * Trimesh<M,V,E,P>          my_trimesh;
 * Quadmesh<M,V,E,P>         my_quadmesh;
 * Polygonmesh<M,V,E,P>      my_polygonmesh;
 * Tetmesh<M,V,E,F,P>        my_tetmesh;
 * Hexmesh<M,V,E,F,P>        my_hexmesh;
 * Polyhedralmesh<M,V,E,F,P> my_hexmesh;
*/

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Bit-wise boolean flags for marking/unmarking mesh elements according to
// various needs. Each mesh element endows a copy of these flags inside its
// properties. Default is zero for everyone. Flags can be accessed as if they
// were booleans with the classical operator[], or with the following methods:
//
//    flags(pos).set()    => sets bit at poistion pos to true (1)
//    flags(pos).reset()  => sets bit at poistion pos to false (0)
//    flags(pos).test()   => returns bit at poistion pos
//    flags(pos).flip()   => inverts the bit at position pos
//    ...
enum
{
    MARKED,       // general flag for global marking. Can be used to mark visited
                  // elements, creases, mesh borders,...). Marked elements can also
                  // be highlighted at rendering time, with dedicated colors/lines.
                  // By default, booundary and non manifold elements are marked at
                  // loading time. The following methods allow to easily unmark all
                  // mesh elements: vert_unmark_all(), edge_unmark_all(), face_unmark_all(), poly_unmark_all()

    MARKED_LOCAL, // general flag for local marking: useful for local methods that
                  // need a temporary flag to operate on mesh elements.
                  // The following methods allow to easily unmark all mesh elements:
                  // vert_local_unmark_near_vert(), vert_local_unmark_near_edge(),
                  // vert_local_unmark_near_face(), vert_local_unmark_near_poly()
                  // Similar methods also exist for edges, faces and polys.

    HIDDEN,       // used by the slicer to show/hide mesh elements when rendering
                  // (this flag is actually used only for poly mesh elements)

    CREASE,       // can be used to mark sharp creases

    UNUSED_0,     // unused flags that can be exploited by various algorithms. For code
    UNUSED_1,     // clarity, I suggest to overload the symbols one wants to use, e.g.:
    UNUSED_2,     //
    UNUSED_3      //    enum { MY_FLAG = UNUSED_0 };
                  //
                  //    my_mesh.vert_data(vid).flags[MY_FLAG] = true;
                  //    my_mesh.vert_data(vid).flags(MY_FLAG).set();
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    std::string filename;
}
Mesh_std_attributes;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    vec3d          normal  = vec3d(0,0,0);
    Color          color   = Color::WHITE();
    vec3d          uvw     = vec3d(0,0,0);
    int            label   = -1;
    float          quality = 0.0;
    std::bitset<8> flags;
}
Vert_std_attributes;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    Color          color = Color::BLACK();
    int            label = -1;
    std::bitset<8> flags;
}
Edge_std_attributes;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    vec3d          normal  = vec3d(0,0,0);
    Color          color   = Color::WHITE();
    int            label   = -1;
    float          quality = 0.0;
    float          AO      = 1.0;
    std::bitset<8> flags;
}
Polygon_std_attributes;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    Color          color   = Color::WHITE();
    int            label   = -1;
    float          quality = 0.0;
    std::bitset<8> flags;
}
Polyhedron_std_attributes;

}

#endif // CINO_MESH_ATTRIBUTES_H
