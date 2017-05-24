/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
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
