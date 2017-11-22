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
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
MeshSlicer<Mesh>::MeshSlicer(Mesh & m)
{
    reset(m);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void MeshSlicer<Mesh>::reset(Mesh & m)
{    
    m.poly_show_all();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void MeshSlicer<Mesh>::update(Mesh & m, const SlicerState & s)
{
    float X_thresh = m.bbox().min[0] + m.bbox().delta()[0] * s.X_thresh;
    float Y_thresh = m.bbox().min[1] + m.bbox().delta()[1] * s.Y_thresh;
    float Z_thresh = m.bbox().min[2] + m.bbox().delta()[2] * s.Z_thresh;

    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        vec3d  c = m.poly_centroid(pid);
        float  q = m.poly_data(pid).quality;
        int    l = m.poly_data(pid).label;

        bool pass_X = (s.X_sign == LEQ) ? (c.x() <=   X_thresh) : (c.x() >=   X_thresh);
        bool pass_Y = (s.Y_sign == LEQ) ? (c.y() <=   Y_thresh) : (c.y() >=   Y_thresh);
        bool pass_Z = (s.Z_sign == LEQ) ? (c.z() <=   Z_thresh) : (c.z() >=   Z_thresh);
        bool pass_Q = (s.Q_sign == LEQ) ? (q     <= s.Q_thresh) : (q     >= s.Q_thresh);
        bool pass_L = (s.L_mode == IS ) ? (l == -1 || l == s.L_filter) : (l == -1 || l != s.L_filter);

        bool b = (s.mode == AND) ? ( pass_X &&  pass_Y &&  pass_Z &&  pass_L &&  pass_Q)
                                 : (!pass_X || !pass_Y || !pass_Z || !pass_L || !pass_Q);

        m.poly_data(pid).visible = b;

        //std::cout << pass_X << " " << pass_Y << " " << pass_Z << " " << pass_Q << " " << pass_L << std::endl;
    }
}

}
