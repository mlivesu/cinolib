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
#include <cinolib/remesh_BotschKobbelt2004.h>
#include <cinolib/tangential_smoothing.h>

namespace cinolib
{

/* This method implements one iteration of the remeshing algorithm
 * described in:
 *
 * A Remeshing Approach to Multiresolution Modeling
 * M.Botsch, L.Kobbelt
 * Symposium on Geomtry Processing, 2004
*/

template<class M, class V, class E, class P>
CINO_INLINE
void remesh_Botsch_Kobbelt_2004(DrawableTrimesh<M,V,E,P> & m,
                                const double               target_edge_length,
                                const bool                 preserve_marked_features)
{
    double l = (target_edge_length>0) ? target_edge_length : m.edge_avg_length();

    // 1) split too long edges
    //
    uint count = 0;
    uint ne = m.num_edges();
    for(uint eid=0; eid<ne; ++eid)
    {
        if (m.edge_length(eid) > 4./3.*l)
        {
            bool mark_children = (preserve_marked_features && m.edge_data(eid).flags[MARKED]);
            uint vid0 = m.edge_vert_id(eid, 0);
            uint vid1 = m.edge_vert_id(eid, 1);
            uint vid  = m.edge_split(eid, 0.5);
            ++count;

            if(mark_children)
            {
                int e0 = m.edge_id(vid,vid0); assert(e0>=0);
                int e1 = m.edge_id(vid,vid1); assert(e1>=0);
                m.edge_data(e0).flags[MARKED] = true;
                m.edge_data(e1).flags[MARKED] = true;
            }
        }
    }
    std::cout << "\t" << count << " edges longer than " << 4./3.*l << " were split." << std::endl;

    // 2) collapse too short edges
    //
    count = 0;
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        bool inc_to_marked = false;
        if(preserve_marked_features)
        {
            uint vid0 = m.edge_vert_id(eid,0);
            uint vid1 = m.edge_vert_id(eid,1);
            for(uint nbr : m.adj_v2e(vid0)) if (m.edge_data(nbr).flags[MARKED]) inc_to_marked = true;
            for(uint nbr : m.adj_v2e(vid1)) if (m.edge_data(nbr).flags[MARKED]) inc_to_marked = true;
        }
        if (preserve_marked_features && inc_to_marked) continue;

        if (m.edge_length(eid) < 4./5.*l)
        {
            m.edge_collapse(eid, 0.5);
            ++count;
        }
    }
    std::cout << "\t" << count << " edges shorter than " << 4./5.*l << " were collapsed." << std::endl;

    // 3) optimize per vert valence
    //
    count = 0;
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        if (preserve_marked_features && m.edge_data(eid).flags[MARKED]) continue;

        std::vector<uint> vopp = m.verts_opposite_to(eid);
        if (vopp.size()!=2) continue;

        uint vid0 = m.edge_vert_id(eid,0);
        uint vid1 = m.edge_vert_id(eid,1);
        uint vid2 = vopp.at(0);
        uint vid3 = vopp.at(1);

        uint val0 = m.vert_valence(vid0);
        uint val1 = m.vert_valence(vid1);
        uint val2 = m.vert_valence(vid2);
        uint val3 = m.vert_valence(vid3);

        uint val_opt0 = m.vert_is_boundary(vid0) ? 4 : 6;
        uint val_opt1 = m.vert_is_boundary(vid1) ? 4 : 6;
        uint val_opt2 = m.vert_is_boundary(vid2) ? 4 : 6;
        uint val_opt3 = m.vert_is_boundary(vid3) ? 4 : 6;

        uint before = (val0 - val_opt0)*(val0 - val_opt0) +
                      (val1 - val_opt1)*(val1 - val_opt1) +
                      (val2 - val_opt2)*(val2 - val_opt2) +
                      (val3 - val_opt3)*(val3 - val_opt3);

        --val0; --val1;
        ++val2; ++val3;

        uint after = (val0 - val_opt0)*(val0 - val_opt0) +
                     (val1 - val_opt1)*(val1 - val_opt1) +
                     (val2 - val_opt2)*(val2 - val_opt2) +
                     (val3 - val_opt3)*(val3 - val_opt3);

        if(before>after) // flip only if minimize sqrd deviation from ideal valence
        {
            P   data    = m.poly_data(m.adj_e2p(eid).front());
            int new_eid = m.edge_flip(eid);

            if(new_eid>=0) // copy per poly attributes in the newly generated poly (but restore right normal!)
            {
                for(uint pid : m.adj_e2p(new_eid))
                {
                    m.poly_data(pid) = data;
                    m.update_p_normal(pid);
                }
                m.update_v_normal(m.edge_vert_id(new_eid,0));
                m.update_v_normal(m.edge_vert_id(new_eid,1));
            }
            ++count;
        }
    }
    std::cout << "\t" << count << " edge flip were performed to normalize vertex valence to 6" << std::endl;


    // 4) relocate vertices by tangential smoothing
    //
    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        bool anchored = false;
        for(uint eid : m.adj_v2e(vid))
        {
            if (preserve_marked_features && m.edge_data(eid).flags[MARKED]) anchored = true;
        }
        if (!anchored) tangential_smoothing(m,vid);
    }
    std::cout << "\ttangential smoothing" << std::endl;
}

}
