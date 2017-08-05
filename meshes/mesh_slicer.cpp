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
#include <cinolib/meshes/mesh_slicer.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
MeshSlicer<Mesh>::MeshSlicer(Mesh & m)
{
    slice_thresh[0] = m.bbox().max.x();
    slice_thresh[1] = m.bbox().max.y();
    slice_thresh[2] = m.bbox().max.z();
    slice_thresh[3] = -1;  // Q
    slice_thresh[4] = -1;  // L
    slice_sign  [0] = LEQ; // X
    slice_sign  [1] = LEQ; // Y
    slice_sign  [2] = LEQ; // Z
    slice_sign  [3] = GEQ; // Q
    slice_mode      = AND;

    m.poly_show_all();
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
    assert(sign == LEQ || sign == GEQ);
    assert(mode == AND || mode == OR);

    slice_mode = mode;

    switch (item)
    {
        case X: slice_thresh[0] = m.bbox().min[0] + m.bbox().delta()[0] * thresh;
                slice_sign[0]   = sign;
                break;
        case Y: slice_thresh[1] = m.bbox().min[1] + m.bbox().delta()[1] * thresh;
                slice_sign[1]   = sign;
                break;
        case Z: slice_thresh[2] = m.bbox().min[2] + m.bbox().delta()[2] * thresh;
                slice_sign[2]   = sign;
                break;
        case Q: slice_thresh[3] = thresh;
                slice_sign[3]   = sign;
                break;
        case L: slice_thresh[4] = thresh; break;
        default: assert(false);
    }
    update(m);       
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void MeshSlicer<Mesh>::update(Mesh & m)
{
    for(uint eid=0; eid<m.num_elems(); ++eid)
    {
        vec3d  c = m.poly_centroid(eid);
        float  q = m.poly_data(eid).quality;
        int    l = m.poly_data(eid).label;

        bool pass_X = (slice_sign[0] == LEQ) ? (c.x() <= slice_thresh[0]) : (c.x() >= slice_thresh[0]);
        bool pass_Y = (slice_sign[1] == LEQ) ? (c.y() <= slice_thresh[1]) : (c.y() >= slice_thresh[1]);
        bool pass_Z = (slice_sign[2] == LEQ) ? (c.z() <= slice_thresh[2]) : (c.z() >= slice_thresh[2]);
        bool pass_Q = (slice_sign[3] == LEQ) ? (q     <= slice_thresh[3]) : (q     >= slice_thresh[3]);
        bool pass_L = (l == -1 ||  l != static_cast<int>(slice_thresh[4]));

        bool b = (slice_mode == AND) ? ( pass_X &&  pass_Y &&  pass_Z &&  pass_L &&  pass_Q)
                                     : (!pass_X || !pass_Y || !pass_Z || !pass_L || !pass_Q);

        m.poly_data(eid).visible = b;

        //std::cout << pass_X << " " << pass_Y << " " << pass_Z << " " << pass_Q << " " << pass_L << std::endl;
    }   
}

}
