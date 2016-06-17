/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#include "cotangent.h"

namespace cinolib
{

// http://stackoverflow.com/questions/3738384/stable-cotangent
//
CINO_INLINE double cot(double x) { return tan(M_PI_2 - x); }


/* Template Specializations.
 *
 * As a reference for the tetmesh version of cotangent weights, see:
 *
 * Gradient field based inhomogeneous volumetric mesh deformation for maxillofacial surgery simulation
 * Sheng-hui Liao, Ruo-feng Tong, Jin-xiang Dong, Fu-dong Zhu
 * Computer & Graphics, 2009
*/


template<>
CINO_INLINE
void cotangent_weights<Trimesh>(const Trimesh       & m,
                                const int             vid,
                                std::vector<int>    & nbrs,
                                std::vector<double> & wgts)
{
    assert(nbrs.empty());
    assert(wgts.empty());

    std::vector<int> edges = m.adj_vtx2edg(vid);

    for(int i=0; i<(int)edges.size(); ++i)
    {
        int eid = edges[i];
        int nbr = m.edge_vertex_id(eid,0);
        if (nbr == vid) nbr = m.edge_vertex_id(eid,1);
        assert(nbr != vid);

        std::vector<int> tris = m.adj_edg2tri(eid);
        assert(tris.size() == 2 || tris.size() == 1);

        double wgt = 0.0;
        for(int j=0; j<(int)tris.size(); ++j)
        {
            double alpha = m.angle_at_vertex(tris[j], m.vertex_opposite_to(tris[j], vid, nbr));
            wgt += cot(alpha);
        }
        wgt = (tris.size() == 2) ? wgt * 0.5 : wgt;

        nbrs.push_back(nbr);
        wgts.push_back(wgt);
    }
}


template<>
CINO_INLINE
void cotangent_weights<Tetmesh>(const Tetmesh       & m,
                                const int             vid,
                                std::vector<int>    & nbrs,
                                std::vector<double> & wgts)
{
    assert(nbrs.empty());
    assert(wgts.empty());

    std::vector<int> edges = m.adj_vtx2edg(vid);

    for(int i=0; i<(int)edges.size(); ++i)
    {
        int eid = edges[i];
        int nbr = m.edge_vertex_id(eid, 0);
        if (nbr == vid) nbr = m.edge_vertex_id(eid, 1);
        assert(nbr != vid);

        std::vector<int> tets = m.adj_edg2tet(eid);

        double wgt = 0.0;
        for(int j=0; j<(int)tets.size(); ++j)
        {
            int    tid       = tets[j];
            int    e_opp     = m.tet_edge_opposite_to(tid, vid, nbr);
            int    f_opp_vid = m.tet_face_opposite_to(tid, vid);
            int    f_opp_nbr = m.tet_face_opposite_to(tid, nbr);
            double l_k       = m.tet_edge_length(tid, e_opp);
            double teta_k    = m.tet_dihedral_angle(tid, f_opp_vid, f_opp_nbr);

            wgt += cot(teta_k) * l_k;
        }
        wgt /= 6.0;

        nbrs.push_back(nbr);
        wgts.push_back(wgt);
    }
}

}
