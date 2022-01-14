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
#include <sstream>

namespace cinolib
{

CINO_INLINE
void MeshSlicer::reset()
{
    X_thresh =  1;
    Y_thresh =  1;
    Z_thresh =  1;
    Q_thresh =  1;
    L_filter = -1;
    X_leq    = true;
    Y_leq    = true;
    Z_leq    = true;
    Q_leq    = true;
    L_is     = true;
    mode_AND = true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void MeshSlicer::deserialize(const std::string & str)
{
    std::stringstream ss(str);
    ss >> X_thresh
       >> Y_thresh
       >> Z_thresh
       >> Q_thresh
       >> L_filter
       >> X_leq
       >> Y_leq
       >> Z_leq
       >> Q_leq
       >> L_is
       >> mode_AND;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::string MeshSlicer::serialize() const
{
    std::stringstream ss;
    ss << X_thresh << " "
       << Y_thresh << " "
       << Z_thresh << " "
       << Q_thresh << " "
       << L_filter << " "
       << X_leq    << " "
       << Y_leq    << " "
       << Z_leq    << " "
       << Q_leq    << " "
       << L_is     << " "
       << mode_AND;
    return ss.str();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void MeshSlicer::slice(AbstractMesh<M,V,E,P> & m)
{
    double X_abs_thresh = m.bbox().min[0] + m.bbox().delta()[0] * (X_thresh);
    double Y_abs_thresh = m.bbox().min[1] + m.bbox().delta()[1] * (Y_thresh);
    double Z_abs_thresh = m.bbox().min[2] + m.bbox().delta()[2] * (Z_thresh);

    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        vec3d c = m.poly_centroid(pid);
        float q = m.poly_data(pid).quality;
        int   l = m.poly_data(pid).label;

        bool pass_X = (X_leq) ? (c.x() <= X_abs_thresh) : (c.x() >= X_abs_thresh);
        bool pass_Y = (Y_leq) ? (c.y() <= Y_abs_thresh) : (c.y() >= Y_abs_thresh);
        bool pass_Z = (Z_leq) ? (c.z() <= Z_abs_thresh) : (c.z() >= Z_abs_thresh);
        bool pass_Q = (Q_leq) ? (q     <= Q_thresh    ) : (q     >= Q_thresh);
        bool pass_L = (L_is ) ? (L_filter==-1 || l == L_filter) : (L_filter == -1 || l != L_filter);

        bool b = (mode_AND) ? ( pass_X &&  pass_Y &&  pass_Z &&  pass_L &&  pass_Q)
                            : (!pass_X || !pass_Y || !pass_Z || !pass_L || !pass_Q);

        m.poly_data(pid).flags[HIDDEN] = !b;

        //std::cout << pass_X << " " << pass_Y << " " << pass_Z << " " << pass_Q << " " << pass_L << std::endl;
    }
}

}
