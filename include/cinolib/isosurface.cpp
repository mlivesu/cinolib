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
#include <cinolib/isosurface.h>
#include <cinolib/cino_inline.h>
#include <cinolib/marching_tets.h>
#include <cinolib/interval.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Isosurface<M,V,E,F,P>::Isosurface(const Tetmesh<M,V,E,F,P> & m,
                                  const double               iso_value,
                                  const bool                 run_marching_tets)
    : iso_value(iso_value)
{
    if(run_marching_tets) marching_tets(m, iso_value, verts, tris, norms);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Trimesh<M,V,E,F> Isosurface<M,V,E,F,P>::export_as_trimesh() const
{
    return Trimesh<M,V,E,F>(verts, tris);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
std::vector<uint> Isosurface<M,V,E,F,P>::tessellate(Tetmesh<M,V,E,F,P> & m) const
{
    std::vector<ipair> splits;
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        uint   v0 = m.edge_vert_id(eid,0);
        uint   v1 = m.edge_vert_id(eid,1);
        double f0 = m.vert_data(v0).uvw[0];
        double f1 = m.vert_data(v1).uvw[0];
        if(is_into_interval<double>(iso_value, f0, f1)) splits.emplace_back(v0,v1);
    }

    std::vector<uint> new_vids;
    for(auto e : splits)
    {
        uint v0  = e.first;
        uint v1  = e.second;
        int  eid = m.edge_id(v0,v1);
        assert(eid>=0);
        if(m.edge_vert_id(eid,0)==v1) std::swap(v0,v1);
        double f0 = m.vert_data(v0).uvw[0];
        double f1 = m.vert_data(v1).uvw[0];
        assert(is_into_interval<double>(iso_value, f0, f1));
        double alpha = std::fabs(iso_value - f0)/fabs(f1 - f0);
        uint vid = m.edge_split(eid, alpha);
        m.vert_data(vid).uvw[0] = iso_value;
        new_vids.push_back(vid);
    }

    return new_vids;
}

}
