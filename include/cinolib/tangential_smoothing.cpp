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
#include <cinolib/tangential_smoothing.h>

namespace cinolib
{

/* These functions implement per vertex tangential smoothing,
 * as described in the paper:
 *
 * A Remeshing Approach to Multiresolution Modeling
 * M.Botsch, L.Kobbelt
 * Symposium on Geomtry Processing, 2004
*/

template<class M, class V, class E, class P>
CINO_INLINE
void tangential_smoothing(Trimesh<M,V,E,P> & m, const uint vid)
{
    if(m.vert_is_boundary(vid)) return;

    vec3d  delta(0,0,0);
    double norm_fact = 0.0;
    for(uint nbr : m.adj_v2v(vid))
    {
        double area = m.vert_area(vid);
        delta += area * m.vert(nbr);
        norm_fact += area;
    }
    delta /= norm_fact;
    delta -= m.vert(vid);
    delta -= m.vert_data(vid).normal * delta.dot(m.vert_data(vid).normal);
    m.vert(vid) += delta;

    // update normals
    for(uint pid : m.adj_v2p(vid)) m.update_p_normal(pid);
    for(uint nbr : m.adj_v2v(vid)) m.update_v_normal(nbr);
    m.update_v_normal(vid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void tangential_smoothing(Trimesh<M,V,E,P> & m)
{
    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        tangential_smoothing(m,vid);
    }
}

}
