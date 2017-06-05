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
#include <cinolib/cotangent.h>

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
void cotangent_weights<Trimesh<>>(const Trimesh<>     & m,
                                  const uint            vid,
                                  std::vector<uint>   & nbrs,
                                  std::vector<double> & wgts)
{
    assert(nbrs.empty());
    assert(wgts.empty());

    std::vector<uint> edges = m.adj_v2e(vid);

    for(uint i=0; i<edges.size(); ++i)
    {
        uint eid = edges[i];
        uint nbr = m.edge_vert_id(eid,0);
        if (nbr == vid) nbr = m.edge_vert_id(eid,1);
        assert(nbr != vid);

        std::vector<uint> faces = m.adj_e2f(eid);
        assert(faces.size() == 2 || faces.size() == 1);

        double wgt = 0.0;
        for(uint j=0; j<faces.size(); ++j)
        {
            double alpha = m.face_angle_at_vert(faces[j], m.vert_opposite_to(faces[j], vid, nbr));
            wgt += cot(alpha);
        }
        wgt = (faces.size() == 2) ? wgt * 0.5 : wgt;

        nbrs.push_back(nbr);
        wgts.push_back(wgt);
    }
}


template<>
CINO_INLINE
void cotangent_weights<Tetmesh<>>(const Tetmesh<>     & m,
                                  const uint            vid,
                                  std::vector<uint>   & nbrs,
                                  std::vector<double> & wgts)
{
    assert(nbrs.empty());
    assert(wgts.empty());

    std::vector<uint> edges = m.adj_v2e(vid);

    for(uint i=0; i<edges.size(); ++i)
    {
        uint eid = edges[i];
        uint nbr = m.edge_vert_id(eid, 0);
        if (nbr == vid) nbr = m.edge_vert_id(eid, 1);
        assert(nbr != vid);

        std::vector<uint> cells = m.adj_e2c(eid);

        double wgt = 0.0;
        for(uint cid : cells)
        {
            uint   e_opp     = m.cell_edge_opposite_to(cid, vid, nbr);
            uint   f_opp_vid = m.cell_face_opposite_to(cid, vid);
            uint   f_opp_nbr = m.cell_face_opposite_to(cid, nbr);
            double l_k       = m.cell_edge_length(cid, e_opp);
            double teta_k    = m.cell_dihedral_angle(cid, f_opp_vid, f_opp_nbr);

            wgt += cot(teta_k) * l_k;
        }
        wgt /= 6.0;

        nbrs.push_back(nbr);
        wgts.push_back(wgt);
    }
}

}
