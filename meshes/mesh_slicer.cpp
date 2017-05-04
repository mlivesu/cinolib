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
MeshSlicer<Mesh>::MeshSlicer(Mesh * m) : m_ptr(m)
{
    slice_thresh [ X ] = m_ptr->bbox().max.x();
    slice_thresh [ Y ] = m_ptr->bbox().max.y();
    slice_thresh [ Z ] = m_ptr->bbox().max.z();
    slice_thresh [ L ] = -1.0;
    slice_thresh [ Q ] = -1.0;
    slice_sign   [ X ] = LEQ;
    slice_sign   [ Y ] = LEQ;
    slice_sign   [ Z ] = LEQ;
    slice_sign   [ L ] = LEQ;
    slice_sign   [ Q ] = LEQ;
    slice_mode         = AND;

    m_ptr->elem_show_all();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void MeshSlicer<Mesh>::update(const float thresh,  // thresh on centroids or quality
                              const int   item,    // X, Y, Z, L, Q
                              const int   sign,    // either LEQ or GEQ
                              const int   mode)    // either AND or OR
{
    assert(m_ptr != NULL);

    switch (item)
    {
        case X:
        case Y:
        case Z: slice_thresh[item] = m_ptr->bbox().min[item] + m_ptr->bbox().delta()[item] * thresh; break;
        case L: slice_thresh[item] = thresh; break;
        default: assert(false);
    }
    slice_sign[item] = sign; assert(sign == LEQ || sign == GEQ);
    slice_mode       = mode; assert(mode == AND || mode == OR);

    update();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void MeshSlicer<Mesh>::update()
{
    assert(m_ptr != NULL);

    for(uint eid=0; eid<m_ptr->num_elems(); ++eid)
    {
        vec3d  c = m_ptr->elem_centroid(eid);
        float  q = m_ptr->elem_data(eid).quality;
        int    l = static_cast<int>(slice_thresh[L]);

        bool pass_X = (slice_sign[X]) ? (c.x() <= slice_thresh[X]) : (c.x() >= slice_thresh[X]);
        bool pass_Y = (slice_sign[Y]) ? (c.y() <= slice_thresh[Y]) : (c.y() >= slice_thresh[Y]);
        bool pass_Z = (slice_sign[Z]) ? (c.z() <= slice_thresh[Z]) : (c.z() >= slice_thresh[Z]);
        bool pass_Q = (slice_sign[Q]) ? (q     <= slice_thresh[Q]) : (q     >= slice_thresh[Q]);
        bool pass_L = (l == -1 || m_ptr->elem_data(eid).label != l);

        m_ptr->elem_data(eid).visible = (slice_mode) ? ( pass_X &&  pass_Y &&  pass_Z &&  pass_L &&  pass_Q)
                                                     : (!pass_X || !pass_Y || !pass_Z || !pass_L || !pass_Q);
    }
}

}
