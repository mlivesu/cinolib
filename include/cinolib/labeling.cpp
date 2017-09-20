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
#include <cinolib/labeling.h>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_labeled_subregion(const Tetmesh<M,V,E,F,P>  & m,
                              const int                   label,
                                    Tetmesh<M,V,E,F,P>  & subm,
                                    std::map<uint,uint> & m2subm_vmap,
                                    std::map<uint,uint> & subm2m_vmap)
{
    m2subm_vmap.clear();
    subm2m_vmap.clear();

    std::vector<vec3d> verts;
    std::vector<uint>  polys;

    uint fresh_id = 0;

    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        if (m.poly_data(pid).label == label)
        {
            for(uint off=0; off<m.verts_per_poly(pid); ++off)
            {
                uint vid   = m.poly_vert_id(pid, off);
                uint vnew  = fresh_id++;
                auto query = m2subm_vmap.find(vid);

                if (query == m2subm_vmap.end())
                {
                    verts.push_back(m.vert(vid));

                    m2subm_vmap[vid] = vnew;
                    subm2m_vmap[vnew] = vid;
                }
                else
                {
                    vnew = query->second;
                    --fresh_id;
                }
                polys.push_back(vnew);
            }
        }
    }

    subm = Tetmesh<M,V,E,F,P>(verts, polys);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_labeled_subregion(const Tetmesh<M,V,E,F,P> & m,
                              const int label,
                                    Tetmesh<M,V,E,F,P> & subm)
{
    std::map<uint,uint> m2subm_vmap, subm2m_vmap;
    export_labeled_subregion(m, label, subm, m2subm_vmap, subm2m_vmap);
}

}

