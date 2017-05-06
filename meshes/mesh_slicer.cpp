/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2017                                                         *
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
#include <cinolib/meshes/mesh_slicer.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
MeshSlicer<Mesh>::MeshSlicer(Mesh & m)
{
    slice_thresh[ X ] = m.bbox().max.x();
    slice_thresh[ Y ] = m.bbox().max.y();
    slice_thresh[ Z ] = m.bbox().max.z();
    slice_thresh[ Q ] = 0.0;
    slice_thresh[ L ] = -1;
    slice_sign  [ X ] = LEQ;
    slice_sign  [ Y ] = LEQ;
    slice_sign  [ Z ] = LEQ;
    slice_sign  [ Q ] = LEQ;
    slice_mode        = AND;

    m.elem_show_all();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void MeshSlicer<Mesh>::update(      Mesh  & m,
                              const float   thresh,  // thresh on centroids or quality
                              const int     item,    // X, Y, Z, L, Q
                              const int     sign,    // either LEQ or GEQ
                              const int     mode)    // either AND or OR
{
    switch (item)
    {
        case X:
        case Y:
        case Z: slice_thresh[item] = m.bbox().min[item] + m.bbox().delta()[item] * thresh; break;
        case L: slice_thresh[item] = thresh; break;
        default: assert(false);
    }
    slice_sign[item] = sign; assert(sign == LEQ || sign == GEQ);
    slice_mode       = mode; assert(mode == AND || mode == OR);

    update(m);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void MeshSlicer<Mesh>::update(Mesh & m)
{
    for(uint eid=0; eid<m.num_elems(); ++eid)
    {
        vec3d  c = m.elem_centroid(eid);
        float  q = m.elem_data(eid).quality;
        int    l = m.elem_data(eid).label;

        bool pass_X = (slice_sign[X] == LEQ) ? (c.x() <= slice_thresh[X]) : (c.x() >= slice_thresh[X]);
        bool pass_Y = (slice_sign[Y] == LEQ) ? (c.y() <= slice_thresh[Y]) : (c.y() >= slice_thresh[Y]);
        bool pass_Z = (slice_sign[Z] == LEQ) ? (c.z() <= slice_thresh[Z]) : (c.z() >= slice_thresh[Z]);
        bool pass_Q = (slice_sign[Q] == LEQ) ? (q     <= slice_thresh[Q]) : (q     >= slice_thresh[Q]);
        bool pass_L = (l == -1 ||  l != static_cast<int>(slice_thresh[L]));

        bool b = (slice_mode == AND) ? ( pass_X &&  pass_Y &&  pass_Z &&  pass_L &&  pass_Q)
                                     : (!pass_X || !pass_Y || !pass_Z || !pass_L || !pass_Q);

        m.elem_data(eid).visible = b;
    }
}

}
