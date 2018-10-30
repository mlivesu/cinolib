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
#include <cinolib/isocontour.h>
#include <cinolib/cino_inline.h>
#include <cinolib/interval.h>
#include <queue>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Isocontour<M,V,E,P>::Isocontour()
{
    iso_value = 0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Isocontour<M,V,E,P>::Isocontour(AbstractPolygonMesh<M,V,E,P> & m, double iso_value) : iso_value(iso_value)
{
    for(uint pid=0; pid<m.num_polys(); ++pid)
    for(uint i=0; i< m.poly_tessellation(pid).size()/3; ++i)
    {
        uint   vid0 = m.poly_tessellation(pid).at(3*i+0);
        uint   vid1 = m.poly_tessellation(pid).at(3*i+1);
        uint   vid2 = m.poly_tessellation(pid).at(3*i+2);

        double f0   = m.vert_data(vid0).uvw[0];
        double f1   = m.vert_data(vid1).uvw[0];
        double f2   = m.vert_data(vid2).uvw[0];

        // There are seven possible cases:
        // 1) the curve coincides with (v0,v1)
        // 2) the curve coincides with (v1,v2)
        // 3) the curve coincides with (v2,v0)
        // 4) the curve enters from (v0,v1) and exits from (v0,v2)
        // 5) the curve enters from (v0,v1) and exits from (v1,v2)
        // 6) the curve enters from (v1,v2) and exits from (v2,v0)
        // 7) the does not pass fromm here

        bool through_v0    = (iso_value == f0);
        bool through_v1    = (iso_value == f1);
        bool through_v2    = (iso_value == f2);
        bool crosses_v0_v1 = is_into_interval<double>(iso_value, f0, f1, true);
        bool crosses_v1_v2 = is_into_interval<double>(iso_value, f1, f2, true);
        bool crosses_v2_v0 = is_into_interval<double>(iso_value, f2, f0, true);

        if (through_v0 && through_v1) // case 1) the curve coincides with (v0,v1)
        {
            segs.push_back(m.vert(vid0));
            segs.push_back(m.vert(vid1));
        }
        else if (through_v1 && through_v2) // case 2) the curve coincides with (v1,v2)
        {
            segs.push_back(m.vert(vid1));
            segs.push_back(m.vert(vid2));
        }
        else if (through_v2 && through_v0) // 3) the curve coincides with (v2,v0)
        {
            segs.push_back(m.vert(vid2));
            segs.push_back(m.vert(vid0));
        }
        else if (crosses_v0_v1 && crosses_v1_v2) // case 4) the curve enters from (v0,v1) and exits from (v0,v2)
        {
            double alpha0 = std::fabs(iso_value - f0)/fabs(f1 - f0);
            double alpha1 = std::fabs(iso_value - f1)/fabs(f2 - f1);
            segs.push_back((1.0-alpha0)*m.vert(vid0) + alpha0*m.vert(vid1));
            segs.push_back((1.0-alpha1)*m.vert(vid1) + alpha1*m.vert(vid2));
        }
        else if (crosses_v0_v1 && crosses_v2_v0) // case 5) the curve enters from (v0,v1) and exits from (v1,v2)
        {
            double alpha0 = std::fabs(iso_value - f0)/fabs(f1 - f0);
            double alpha1 = std::fabs(iso_value - f2)/fabs(f0 - f2);
            segs.push_back((1.0-alpha0)*m.vert(vid0) + alpha0*m.vert(vid1));
            segs.push_back((1.0-alpha1)*m.vert(vid2) + alpha1*m.vert(vid0));
        }
        else if (crosses_v1_v2 && crosses_v2_v0) // 6) the curve enters from (v1,v2) and exits from (v2,v0)
        {
            double alpha0 = std::fabs(iso_value - f1)/fabs(f2 - f1);
            double alpha1 = std::fabs(iso_value - f2)/fabs(f0 - f2);
            segs.push_back((1.0-alpha0)*m.vert(vid1) + alpha0*m.vert(vid2));
            segs.push_back((1.0-alpha1)*m.vert(vid2) + alpha1*m.vert(vid0));
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> Isocontour<M,V,E,P>::tessellate(Trimesh<M,V,E,P> & m) const
{
    typedef std::pair<uint,double> split_data;
    std::set<split_data,std::greater<split_data>> edges_to_split; // from highest to lowest id

    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        double f0 = m.vert_data(m.edge_vert_id(eid,0)).uvw[0];
        double f1 = m.vert_data(m.edge_vert_id(eid,1)).uvw[0];

        if (is_into_interval<double>(iso_value, f0, f1))
        {
            double alpha = std::fabs(iso_value - f0)/fabs(f1 - f0);
            edges_to_split.insert(std::make_pair(eid,alpha));
        }
    }

    std::vector<uint> new_vids;
    for(auto e : edges_to_split)
    {
        uint vid = m.edge_split(e.first, e.second);
        m.vert_data(vid).uvw[0] = iso_value;
        new_vids.push_back(vid);
    }

    return new_vids;
}

}
