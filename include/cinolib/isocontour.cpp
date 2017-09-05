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
#include <cinolib/isocontour.h>
#include <queue>

namespace cinolib
{

CINO_INLINE
bool is_into_interval(double v, double bound_0, double bound1)
{
    if (v >= bound_0 && v <= bound1) return true;
    if (v <= bound_0 && v >= bound1) return true;
    return false;
}

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

        bool is_v0 = (iso_value == f0);
        bool is_v1 = (iso_value == f1);
        bool is_v2 = (iso_value == f2);
        bool crosses_v0_v1 = is_into_interval(iso_value, f0, f1);
        bool crosses_v1_v2 = is_into_interval(iso_value, f1, f2);
        bool crosses_v2_v0 = is_into_interval(iso_value, f2, f0);

        if (is_v0 && is_v1)
        {
            segs.push_back(m.vert(vid0));
            segs.push_back(m.vert(vid1));
        }
        else if (is_v1 && is_v2)
        {
            segs.push_back(m.vert(vid1));
            segs.push_back(m.vert(vid2));
        }
        else if (is_v2 && is_v0)
        {
            segs.push_back(m.vert(vid2));
            segs.push_back(m.vert(vid0));
        }
        else if (crosses_v0_v1 && crosses_v1_v2)
        {
            double alpha0 = std::fabs(iso_value - f0)/fabs(f1 - f0);
            double alpha1 = std::fabs(iso_value - f1)/fabs(f2 - f1);
            segs.push_back((1.0-alpha0)*m.vert(vid0) + alpha0*m.vert(vid1));
            segs.push_back((1.0-alpha1)*m.vert(vid1) + alpha1*m.vert(vid2));
        }
        else if (crosses_v0_v1 && crosses_v2_v0)
        {
            double alpha0 = std::fabs(iso_value - f0)/fabs(f1 - f0);
            double alpha1 = std::fabs(iso_value - f2)/fabs(f0 - f2);
            segs.push_back((1.0-alpha0)*m.vert(vid0) + alpha0*m.vert(vid1));
            segs.push_back((1.0-alpha1)*m.vert(vid2) + alpha1*m.vert(vid0));
        }
        else if (crosses_v1_v2 && crosses_v2_v0)
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

        if (is_into_interval(iso_value, f0, f1))
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
