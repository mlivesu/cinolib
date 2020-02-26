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

        m.poly_data(pid).flags[HIDDEN] = !b;

        //std::cout << pass_X << " " << pass_Y << " " << pass_Z << " " << pass_Q << " " << pass_L << std::endl;
    }
}

}
