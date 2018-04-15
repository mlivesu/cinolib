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
