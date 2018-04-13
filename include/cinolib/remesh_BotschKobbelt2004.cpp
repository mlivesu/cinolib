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
#include <cinolib/remesh_BotschKobbelt2004.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void remesh_Botsch_Kobbelt_2004(Trimesh<M,V,E,P> & m,
                                const double       target_edge_length,
                                const bool         preserve_marked_features,
                                const uint         n_iter)
{
    double l = (target_edge_length>0) ? target_edge_length : m.edge_avg_length();

    typedef std::pair<uint,double> split_data;
    std::set<split_data,std::greater<split_data>> edge_list;    // from highest to lowest id

    for(uint i=0; i<n_iter; ++i)
    {
        std::cout << "Remeshing iter #" << i << std::endl;

        // 1) split all edges that are longer than 4/3*l
        //
        for(uint eid=0; eid<m.num_edges(); ++eid)
        {
            if (m.edge_length(eid) > 4./3.*l) edge_list.insert(std::make_pair(eid,0.5));
        }
        std::cout << "\t" << edge_list.size() << " edges split" << std::endl;
        for(auto e : edge_list) m.edge_split(e.first, e.second);
        edge_list.clear();


        // 2) collapse all edges that are shorter than 4/5*l
        //
        uint count = 0;
        for(uint eid=0; eid<m.num_edges(); ++eid)
        {
            if (m.edge_length(eid) < 4./5.*l)
            {
                m.edge_collapse(eid, 0.5);
                ++count;
            }
        }
        std::cout << "\t" << count << " edges collapsed" << std::endl;


        // 3) flip edges in order to minimize the deviation from valence 6 (or 4 on boundaries).
        //
        count = 0;
        for(uint eid=0; eid<m.num_edges(); ++eid)
        {
            uint A = m.vert_valence(m.edge_vert_id(eid,0));
            uint B = m.vert_valence(m.edge_vert_id(eid,1));
            std::vector<uint> vopp = m.verts_opposite_to(eid);
            if (vopp.size()!=2) continue;
            uint C = m.vert_valence(vopp.at(0));
            uint D = m.vert_valence(vopp.at(1));

            if((A-6)*(A-6)+(B-6)*(B-6)+(C-6)*(C-6)+(D-6)*(D-6)>
               (A-5)*(A-5)+(B-5)*(B-5)+(C-7)*(C-7)+(D-7)*(D-7))
            {
                m.edge_flip(eid);
                ++count;
            }
        }
        std::cout << "\t" << count << " edges flips" << std::endl;
    }
}

}
