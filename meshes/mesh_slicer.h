/*********************************************************************************
*  Copyright(C) 2016: Marco Livesu                                               *
*  All rights reserved.                                                          *
*                                                                                *
*  This file is part of CinoLib                                                  *
*                                                                                *
*  CinoLib is dual-licensed:                                                     *
*                                                                                *
*   - For non-commercial use you can redistribute it and/or modify it under the  *
*     terms of the GNU General Public License as published by the Free Software  *
*     Foundation; either version 3 of the License, or (at your option) any later *
*     version.                                                                   *
*                                                                                *
*   - If you wish to use it as part of a commercial software, a proper agreement *
*     with the Author(s) must be reached, based on a proper licensing contract.  *
*                                                                                *
*  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE       *
*  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.     *
*                                                                                *
*  Author(s):                                                                    *
*                                                                                *
*     Marco Livesu (marco.livesu@gmail.com)                                      *
*     http://pers.ge.imati.cnr.it/livesu/                                        *
*                                                                                *
*     Italian National Research Council (CNR)                                    *
*     Institute for Applied Mathematics and Information Technologies (IMATI)     *
*     Via de Marini, 6                                                           *
*     16149 Genoa,                                                               *
*     Italy                                                                      *
**********************************************************************************/
#ifndef CINO_MESH_SLICER_H
#define CINO_MESH_SLICER_H

#include <cinolib/cinolib.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/gl/draw_tris_quads.h>

namespace cinolib
{
/* Filter mesh elements according to a number of different criteria.
 * Useful to inspect the interior of volume meshes, or to isolate
 * interesting portions of a complex surface mesh.
*/
template<class Mesh>
class MeshSlicer
{
    public:

        MeshSlicer() {}

        MeshSlicer(Mesh & m);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update(      Mesh  & m,
                    const float   thresh,  // thresh on centroid
                    const int     item,    // X, Y, Z, Q, L
                    const int     sign,    // either LEQ or GEQ (used for X Y Z Q)
                    const int     mode);   // either AND or OR

        void update(Mesh & m);

    protected:


        float slice_thresh[5]; // X Y Z Q L
        int   slice_sign  [4]; // either LEQ or GEQ (used for X Y Z Q)
        int   slice_mode;      // either AND or OR
};

}

#ifndef  CINO_STATIC_LIB
#include "mesh_slicer.cpp"
#endif

#endif // CINO_MESH_SLICER_H
