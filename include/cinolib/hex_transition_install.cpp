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
*     Luca Pitzalis (lucapizza@gmail.com)                                       *
*     University of Cagliari                                                    *
*                                                                               *
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
#include <cinolib/hex_transition_install.h>
#include <cinolib/hex_transition_orient.h>

namespace cinolib
{

namespace // anonymous
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//Custom comparator operator for maps of vec3d
struct vert_compare
{
    bool operator()(const vec3d & a,
                    const vec3d & b) const
    {
       double eps = 1e-6;
       if(a.x()-b.x() < 0.0 && abs(a.x()-b.x()) > eps)
       {
           return true;
       }
       else if(abs(a.x()-b.x()) < eps)
       {
           if(a.y()-b.y() < 0.0 && abs(a.y()-b.y()) > eps)
           {
               return true;
           }
           else if(abs(a.y()-b.y()) < eps)
           {
               if(a.z()-b.z() < 0.0 && abs(a.z()-b.z()) > eps)
               {
                   return true;
               }
           }
       }
       return false;
    }
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint edge_orientation(const Polyhedralmesh<M,V,E,F,P> & m, const uint vid, const uint eid)
{
    vec3d v1 = m.vert(vid);
    vec3d v2 = m.edge_vert_id(eid, 0) == vid ? m.edge_vert(eid, 1) : m.edge_vert(eid, 0);
    vec3d n  = v2 - v1; n.normalize();
    for(uint i=0; i<3; i++) n[i] = std::round(n[i]);
    if(n.x() ==  1)  return PLUS_X;
    if(n.x() == -1)  return MINUS_X;
    if(n.y() ==  1)  return PLUS_Y;
    if(n.y() == -1)  return MINUS_Y;
    if(n.z() ==  1)  return PLUS_Z;
    if(n.z() == -1)  return MINUS_Z;
    assert(false);
    return 0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
int find_min_ref(const Polyhedralmesh<M,V,E,F,P> & m, const uint vid)
{
    int min_ref = max_int;
    for(uint pid : m.adj_v2p(vid))
    {
        if(m.poly_data(pid).label < min_ref)
        {
            min_ref = m.poly_data(pid).label;
        }
    }
    return min_ref;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
int find_max_ref(const Polyhedralmesh<M,V,E,F,P> & m, const uint vid)
{
    int max_ref = -1;
    for(uint pid : m.adj_v2p(vid))
    {
        if(m.poly_data(pid).label > max_ref)
        {
            max_ref = m.poly_data(pid).label;
        }
    }
    return max_ref;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void get_transition_verts_direction(const Polyhedralmesh<M,V,E,F,P> & m,
                                    const std::vector<bool>         & transition_verts,
                                          std::vector<uint>         & transition_verts_direction)
{
    transition_verts_direction = std::vector<uint>(m.num_verts());

    std::map<vec3d, uint, vert_compare> v_map;
    for(uint vid=0; vid<m.num_verts(); vid++) v_map[m.vert(vid)] = vid;

    for(uint vid=0; vid<m.num_verts(); vid++)
    {
        if(transition_verts[vid] && m.adj_v2p(vid).size() == 8)
        {
            double length = 0;
            for(uint eid : m.adj_v2e(vid))
            {
                if(m.adj_e2p(eid).size() == 4 && m.edge_length(eid) > length)
                {
                    length = m.edge_length(eid);
                    transition_verts_direction[vid] = edge_orientation(m, vid, eid);
                }
            }
        }
        else if(transition_verts[vid])
        {
            double length = 0;
            for(uint eid : m.adj_v2e(vid))
            {
                if(m.edge_length(eid) > length && v_map.find(m.edge_sample_at(eid, 0.5)) == v_map.end())
                {
                    length = m.edge_length(eid);
                    transition_verts_direction[vid] = edge_orientation(m, vid, eid);
                }
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void mark_concave_vert(const Polyhedralmesh<M,V,E,F,P>     & m,
                       const std::vector<uint>             & t_verts,
                       const std::vector<uint>             & t_verts_direction,
                       std::unordered_map<uint,SchemeInfo> & poly2scheme)
{
    assert(t_verts.size() == 3);
    int min_ref = find_min_ref(m, t_verts[0]);

    std::set<uint> adj_polys_cluster;
    for(uint vid : t_verts)
    {
        for(uint pid : m.adj_v2p(vid))
        {
            if(m.poly_data(pid).label == min_ref)
            {
                adj_polys_cluster.insert(pid);
            }
        }
    }

    //FIND VERT
    std::set<uint> flats = adj_polys_cluster;
    uint concave = 0;
    uint conc_vert=0;
    std::vector<uint> adjs_v;

    for(uint pid : adj_polys_cluster)
    {
        if(m.poly_contains_vert(pid, t_verts[0]) &&
           m.poly_contains_vert(pid, t_verts[1]) &&
           m.poly_contains_vert(pid, t_verts[2]))
        {
            concave = pid;
            auto poly_verts = m.poly_verts_id(pid);
            std::set<uint> poly_vids_set(poly_verts.begin(), poly_verts.end());
            for(uint t_vert : t_verts)
            {
                for(uint vid : m.adj_v2v(t_vert))
                {
                    if(m.poly_contains_vert(pid, vid)) poly_vids_set.erase(vid);
                }
                poly_vids_set.erase(t_vert);
            }

            assert(poly_vids_set.size() == 1);

            conc_vert = *poly_vids_set.begin();
            for(uint vid : m.adj_v2v(conc_vert))
            {
                if(m.poly_contains_vert(concave, vid))
                {
                    adjs_v.push_back(vid);
                }
            }

            assert(adjs_v.size() == 3);
            break;
        }
    }
    std::map<uint, bool> v2cut;
    auto query = poly2scheme.find(concave);
    if(query == poly2scheme.end() || query->second.type == HexTransition::FLAT        ||
                                     query->second.type == HexTransition::FLAT_CONVEX ||
                                     query->second.type == HexTransition::VERT_SIDE   ||
                                     query->second.type == HexTransition::VERT_SIDE_WB)
    {
        SchemeInfo info;
        info.type  = HexTransition::VERT_CENTER;
        info.scale = m.edge_length(m.adj_p2e(concave)[0]);

        uint num_cuts=0;
        for(uint vid : adjs_v)
        {
            uint eid = m.edge_id(conc_vert, vid);
            uint ori = edge_orientation(m, conc_vert, eid);
            ori = ori > 12 ? ori-3 : ori;

            vec3d middle = m.edge_sample_at(eid, 0.5);
            int middle_vid = -1;
            for(uint adj_v : m.adj_v2v(vid))
            {
                if(eps_eq(m.vert(adj_v), middle))
                {
                    middle_vid = adj_v;
                    break;
                }
            }
            assert(middle_vid != -1);

            std::set<uint> refs;
            for(uint adj : m.adj_v2p(middle_vid))
            {
                refs.insert(m.poly_data(adj).label);
            }

            if(refs.size() > 1)
            {
                num_cuts++;
                v2cut[vid] = true;
                info.cuts[ori] = true;
            }
            else
            {
                v2cut[vid] = false;
                info.cuts[ori] = false;
            }
        }

        if(num_cuts == 1)      info.type = HexTransition::VERT_CENTER_WB_1;
        else if(num_cuts == 2) info.type = HexTransition::VERT_CENTER_WB_2;
        else if(num_cuts == 3) info.type = HexTransition::VERT_CENTER_WB_3;

        info.scheme_type = SchemeType::CORN_S;
        info.orientations.push_back(t_verts_direction[t_verts[0]]);
        info.orientations.push_back(t_verts_direction[t_verts[1]]);
        info.orientations.push_back(t_verts_direction[t_verts[2]]);
        info.t_verts.push_back(m.vert(conc_vert));
        poly2scheme[concave] = info;
    }

    flats.erase(concave);

    uint lateral_y = MINUS_Y;
    if(t_verts_direction[t_verts[0]] == PLUS_Y || t_verts_direction[t_verts[1]] == PLUS_Y || t_verts_direction[t_verts[2]] == PLUS_Y) lateral_y = PLUS_Y;

    //FIND SIDES
    for(uint pid : m.adj_p2p(concave))
    {
        auto query = poly2scheme.find(pid);
        if(query == poly2scheme.end() ||
          (query->second.scheme_type == SchemeType::CORN_S       &&
          (query->second.type != HexTransition::VERT_CENTER      &&
           query->second.type != HexTransition::VERT_CENTER_WB_1 &&
           query->second.type != HexTransition::VERT_CENTER_WB_2 &&
           query->second.type != HexTransition::VERT_CENTER_WB_3  )))
        {
            SchemeInfo info;

            for(auto &p : v2cut)
            {
                if(m.poly_contains_vert(pid, p.first))
                {
                    bool is_cut = false;

                    for(uint vid : m.adj_v2v(p.first))
                    {
                        if(m.poly_contains_vert(pid, vid))
                        {
                            uint eid = m.edge_id(p.first, vid);
                            vec3d middle = m.edge_sample_at(eid, 0.5);
                            int middle_vid = -1;
                            for(uint adj_v : m.adj_v2v(p.first))
                            {
                                if(eps_eq(m.vert(adj_v), middle))
                                {
                                    middle_vid = adj_v;
                                    break;
                                }
                            }
                            assert(middle_vid != -1);
                            std::set<uint> refs;
                            for(uint adj : m.adj_v2p(middle_vid))
                            {
                                refs.insert(m.poly_data(adj).label);
                            }
                            if(refs.size() > 1)
                            {
                                is_cut = true;
                                break;
                            }
                        }
                    }
                    if(is_cut) info.type = HexTransition::VERT_SIDE_WB;
                    else       info.type = HexTransition::VERT_SIDE;
                }
            }
            //info.type = poly2scheme[concave].type == CONC_VERT ? CONC_VERT_LATERAL : CONC_VERT_LATERAL_MR;
            info.scheme_type = SchemeType::CORN_S;
            info.scale = m.edge_length(m.adj_p2e(concave)[0]);

            for(uint vid : m.face_verts_id(m.poly_shared_face(pid, concave)))
            {
                if(vid == t_verts[0] || vid == t_verts[1] || vid == t_verts[2])
                {
                    info.orientations.push_back(t_verts_direction[vid]);
                }
                else
                {
                    uint curr_ref = m.poly_data(pid).label;
                    std::set<uint> refs;
                    for(uint adj : m.adj_v2p(vid)){
                        if(m.poly_data(adj).label >= curr_ref){
                            refs.insert(m.poly_data(adj).label);
                        }
                    }
                    if(refs.size() >= 2) info.t_verts.push_back(m.vert(vid));
                }
            }
            info.orientations.push_back(lateral_y);
            poly2scheme[pid] = info;
        }
        flats.erase(pid);
    }

    //FIND FLATS
    for(uint pid : flats)
    {
        if(poly2scheme.find(pid) == poly2scheme.end())
        {
            SchemeInfo info;
            info.type        = HexTransition::FLAT;
            info.scheme_type = SchemeType::CORN_S;
            info.scale       = m.edge_length(m.adj_p2e(concave)[0]);
            if(m.poly_contains_vert(pid, t_verts[0]))
            {
                info.orientations.push_back(t_verts_direction[t_verts[0]]);
                info.orientations.push_back(t_verts_direction[t_verts[1]]);
                info.orientations.push_back(t_verts_direction[t_verts[2]]);
                info.t_verts.push_back(m.vert(t_verts[0]));
            }
            else if(m.poly_contains_vert(pid, t_verts[1]))
            {
                info.orientations.push_back(t_verts_direction[t_verts[1]]);
                info.orientations.push_back(t_verts_direction[t_verts[0]]);
                info.orientations.push_back(t_verts_direction[t_verts[2]]);
                info.t_verts.push_back(m.vert(t_verts[1]));
            }
            else if(m.poly_contains_vert(pid, t_verts[2]))
            {
                info.orientations.push_back(t_verts_direction[t_verts[2]]);
                info.orientations.push_back(t_verts_direction[t_verts[0]]);
                info.orientations.push_back(t_verts_direction[t_verts[1]]);
                info.t_verts.push_back(m.vert(t_verts[2]));
            }
            poly2scheme[pid] = info;
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void mark_concave_edge(const Polyhedralmesh<M,V,E,F,P>     & m,
                       const std::vector<uint>             & t_verts,
                       const std::vector<uint>             & t_verts_direction,
                       std::unordered_map<uint,SchemeInfo> & poly2scheme)
{
    auto adjs_v1 = m.adj_v2v(t_verts[0]);
    auto adjs_v2 = m.adj_v2v(t_verts[1]);
    std::vector<uint> intersection;
    std::sort(adjs_v1.begin(), adjs_v1.end());
    std::sort(adjs_v2.begin(), adjs_v2.end());
    std::set_intersection(adjs_v1.begin(), adjs_v1.end(), adjs_v2.begin(), adjs_v2.end(), std::back_inserter(intersection));

    assert(intersection.size() == 2 || intersection.size() == 1);

    int ref = 0;
    for(uint pid : m.adj_v2p(t_verts[0])){
        if(m.poly_contains_vert(pid, t_verts[1])){
            ref = m.poly_data(pid).label;
            break;
        }
    }

    uint conc_edge_vid = 0;
    bool nested_candidate = false;
    for(uint vid : intersection)
    {
        std::set<int> refs;
        for(uint pid : m.adj_v2p(vid))
        {
            if(ref <= m.poly_data(pid).label)
                refs.insert(m.poly_data(pid).label);
        }
        if(refs.size() == 2)
        {
            conc_edge_vid = vid;
            break;
        }
        else if(refs.size() == 3)
        {
            conc_edge_vid = vid;
            nested_candidate = true;
            break;
        }
    }

    std::vector<uint> concaves;
    int min_ref = find_min_ref(m, conc_edge_vid);
    int orient = -1;

    for(uint pid : m.adj_v2p(conc_edge_vid))
    {
        if(!(m.poly_contains_vert(pid, t_verts[0]) && m.poly_contains_vert(pid, t_verts[1]))) continue;
        if(m.poly_data(pid).label == min_ref)
        {
            auto query = poly2scheme.find(pid);
            if(query == poly2scheme.end() || query->second.type == HexTransition::FLAT || query->second.type == HexTransition::FLAT_CONVEX){

                int eid_to_check = -1;
                for(uint eid : m.adj_v2e(conc_edge_vid))
                {
                    if(m.poly_contains_edge(pid, eid)                                           &&
                       edge_orientation(m, conc_edge_vid, eid) != t_verts_direction[t_verts[0]] &&
                       edge_orientation(m, conc_edge_vid, eid) != t_verts_direction[t_verts[1]])
                    {
                        orient = edge_orientation(m, conc_edge_vid, eid);
                        eid_to_check = eid;
                        break;
                    }
                }

                vec3d middle = m.edge_sample_at(eid_to_check, 0.5);
                int middle_vid = -1;
                for(uint adj_v : m.adj_v2v(conc_edge_vid))
                {
                    if(eps_eq(m.vert(adj_v), middle))
                    {
                        middle_vid = adj_v;
                        break;
                    }
                }
                assert(middle_vid != -1);
                bool is_nested = false;
                std::set<uint> refs;
                for(uint adj : m.adj_v2p(middle_vid))
                {
                    refs.insert(m.poly_data(adj).label);
                }
                if(refs.size() > 1)
                {
                    is_nested = true;
                }

                concaves.push_back(pid);

                SchemeInfo info;
                info.type = is_nested ? HexTransition::EDGE_WB : HexTransition::EDGE;
                info.scale = m.edge_length(m.adj_p2e(pid)[0]);
                info.t_verts.push_back(m.vert(conc_edge_vid));
                info.orientations.push_back(orient);
                info.scheme_type = SchemeType::CONC_S;
                poly2scheme[pid] = info;
            }
        }
    }

    for(uint i=0; i<t_verts.size(); i++)
    {
        for(uint pid : m.adj_v2p(t_verts[i]))
        {
            if(m.poly_contains_vert(pid, conc_edge_vid) || m.poly_data(pid).label != min_ref) continue;
            if(poly2scheme.find(pid) == poly2scheme.end())
            {
                SchemeInfo info;
                info.type = HexTransition::FLAT;

                for(uint adj : m.adj_p2p(pid))
                {
                    if(m.poly_contains_vert(adj, t_verts[i])) continue;

                    auto query = poly2scheme.find(adj);
                    if(query != poly2scheme.end() && (query->second.type == HexTransition::EDGE_WB      ||
                                                      query->second.type == HexTransition::EDGE         ||
                                                      query->second.type == HexTransition::VERT_SIDE    ||
                                                      query->second.type == HexTransition::VERT_SIDE_WB ||
                                                      query->second.type == HexTransition::FLAT_CONVEX))
                    {
                        info.type = HexTransition::FLAT_CONVEX;
                    }
                }

                info.scale = m.edge_length(m.adj_p2e(pid)[0]);
                info.t_verts.push_back(m.vert(t_verts[i]));
                info.orientations.push_back(t_verts_direction[t_verts[i]]);
                info.orientations.push_back(t_verts_direction[t_verts[(i+1)%t_verts.size()]]);
                info.scheme_type = SchemeType::CONC_S;
                poly2scheme[pid] = info;
            }
        }
    }

   //cut lateral flats if they exist

    for(uint concave : concaves)
    {
        for(uint adj : m.adj_p2p(concave))
        {
            auto query = poly2scheme.find(adj);
            if(query != poly2scheme.end())
            {
                if(m.poly_contains_vert(adj, t_verts[0]) || m.poly_contains_vert(adj, t_verts[1])) continue;

                if(query->second.type == HexTransition::FLAT)
                {
                    poly2scheme[adj].type = HexTransition::FLAT_CONVEX;
                }
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void mark_convex(const Polyhedralmesh<M,V,E,F,P>     & m,
                 const std::vector<uint>             & t_verts,
                 const std::vector<uint>             & t_verts_direction,
                 std::unordered_map<uint,SchemeInfo> & poly2scheme)
{
    uint conv_edge_vert = t_verts.back();
    int min_ref = find_min_ref(m, conv_edge_vert);

    auto adj1 = m.adj_v2p(t_verts[0]);
    auto adj2 = m.adj_v2p(t_verts[1]);
    std::vector<uint> intersection;
    std::sort(adj1.begin(), adj1.end());
    std::sort(adj2.begin(), adj2.end());
    std::set_intersection(adj1.begin(), adj1.end(), adj2.begin(), adj2.end(), std::back_inserter(intersection));
    if(intersection.size() > 0)
    {
        return;
    }

    for(uint pid : m.adj_v2p(conv_edge_vert))
    {
        if(m.poly_data(pid).label == min_ref)
        {
            if(m.poly_contains_vert(pid, t_verts[0]) || m.poly_contains_vert(pid, t_verts[1])) continue;

            auto query = poly2scheme.find(pid);
            if(query == poly2scheme.end())
            {
                SchemeInfo info;
                info.type = HexTransition::CONVEX_1;

                int orient = -1;
                for(uint eid : m.adj_v2e(conv_edge_vert))
                {
                    if(m.poly_contains_edge(pid, eid)                                            &&
                       edge_orientation(m, conv_edge_vert, eid) != t_verts_direction[t_verts[0]] &&
                       edge_orientation(m, conv_edge_vert, eid) != t_verts_direction[t_verts[1]])
                    {
                        orient = edge_orientation(m, conv_edge_vert, eid);
                        break;
                    }

                }

                info.scheme_type = SchemeType::CONV_S;
                info.cuts[orient] = true;
                info.scale = m.edge_length(m.adj_p2e(pid)[0]);
                info.t_verts.push_back(m.vert(conv_edge_vert));
                info.orientations.push_back(orient);
                poly2scheme[pid] = info;
            }
            else
            {
                if(query->second.type == HexTransition::CONVEX_1)
                {
                    poly2scheme[pid].type = HexTransition::CONVEX_2;
                    int orient = -1;
                    for(uint eid : m.adj_v2e(conv_edge_vert))
                    {
                        if(m.poly_contains_edge(pid, eid)                                            &&
                           edge_orientation(m, conv_edge_vert, eid) != t_verts_direction[t_verts[0]] &&
                           edge_orientation(m, conv_edge_vert, eid) != t_verts_direction[t_verts[1]])
                        {
                            orient = edge_orientation(m, conv_edge_vert, eid);
                            break;
                        }
                    }
                    poly2scheme[pid].cuts[orient] = true;
                    poly2scheme[pid].t_verts.push_back(m.vert(conv_edge_vert));
                }
                else if(query->second.type == HexTransition::CONVEX_2)
                {
                    poly2scheme[pid].type = HexTransition::CONVEX_3;
                    int orient = -1;
                    for(uint eid : m.adj_v2e(conv_edge_vert))
                    {
                        if(m.poly_contains_edge(pid, eid)                                            &&
                           edge_orientation(m, conv_edge_vert, eid) != t_verts_direction[t_verts[0]] &&
                           edge_orientation(m, conv_edge_vert, eid) != t_verts_direction[t_verts[1]])
                        {
                            orient = edge_orientation(m, conv_edge_vert, eid);
                            break;
                        }

                    }
                    poly2scheme[pid].cuts[orient] = true;
                    poly2scheme[pid].t_verts.push_back(m.vert(conv_edge_vert));
                }
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void mark_flat(const Polyhedralmesh<M,V,E,F,P>     & m,
               uint                                & t_vert,
               const std::vector<uint>             & orientation,
               std::unordered_map<uint,SchemeInfo> & poly2scheme)
{
    int min_ref = find_min_ref(m, t_vert);

    for(uint pid : m.adj_v2p(t_vert))
    {
        if(m.poly_data(pid).label == min_ref)
        {
            if(poly2scheme.find(pid) == poly2scheme.end())
            {
                SchemeInfo info;
                info.type = HexTransition::FLAT;

                info.scale = m.edge_length(m.adj_p2e(pid)[0]);
                info.orientations.push_back(orientation[t_vert]);
                info.t_verts.push_back(m.vert(t_vert));
                info.scheme_type = SchemeType::FLAT_S;
                poly2scheme[pid] = info;

            }
            else if(poly2scheme[pid].type == HexTransition::FLAT     ||
                    poly2scheme[pid].type == HexTransition::CONVEX_1 ||
                    poly2scheme[pid].type == HexTransition::CONVEX_2 ||
                    poly2scheme[pid].type == HexTransition::CONVEX_3)
            {
                SchemeInfo info;
                info.type = HexTransition::FLAT_CONVEX;
                info.scale = m.edge_length(m.adj_p2e(pid)[0]);
                info.orientations.push_back(orientation[t_vert]);
                info.t_verts.push_back(m.vert(t_vert));
                info.scheme_type = SchemeType::FLAT_S;
                poly2scheme[pid] = info;

                //ADJSUST ADJACENTS
                for(uint adj : m.adj_p2p(pid))
                {
                    auto query = poly2scheme.find(adj);
                    if(query != poly2scheme.end() &&
                       query->second.type == HexTransition::FLAT &&
                      (query->second.scheme_type == SchemeType::CONC_S ||
                       query->second.scheme_type == SchemeType::CORN_S))
                    {
                        poly2scheme[adj].type = HexTransition::FLAT_CONVEX;
                    }
                }
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void cut_flats(const Polyhedralmesh<M,V,E,F,P>     & m,
               std::unordered_map<uint,SchemeInfo> & poly2scheme)
{
    std::deque<uint> queue;
    for(auto &p : poly2scheme){
        queue.push_back(p.first);
    }

    while(!queue.empty())
    {
        uint id = queue.front();
        queue.pop_front();
        auto p = *poly2scheme.find(id);
        if(p.second.type == HexTransition::FLAT_CONVEX)
        {
            for(uint adj : m.adj_p2p(p.first))
            {
                auto query = poly2scheme.find(adj);
                if(query != poly2scheme.end() &&
                   query->second.type == HexTransition::FLAT       &&
                  (query->second.scheme_type == SchemeType::CONC_S ||
                   query->second.scheme_type == SchemeType::CORN_S))
                {
                    if(!eps_eq(p.second.t_verts[0], query->second.t_verts[0]))
                    {
                        poly2scheme[adj].type = HexTransition::FLAT_CONVEX;
                        queue.push_back(adj);
                    }
                }
            }
        }

        if(p.second.type ==        HexTransition::FLAT &&
           p.second.scheme_type == SchemeType::CORN_S)
        {
            uint num_adj_conc_lateral = 0;
            for(uint adj : m.adj_p2p(p.first))
            {
                auto query = poly2scheme.find(adj);
                if(query != poly2scheme.end()                     &&
                  (query->second.type == HexTransition::VERT_SIDE ||
                   query->second.type == HexTransition::VERT_SIDE_WB))
                {
                   num_adj_conc_lateral++;
                }
            }
            if(num_adj_conc_lateral > 2)
            {
                 poly2scheme[p.first].type = HexTransition::FLAT_CONVEX;
                 queue.push_back(p.first);
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void merge_schemes_into_mesh(Polyhedralmesh<M,V,E,F,P>           & m,
                             std::unordered_map<uint,SchemeInfo> & poly2scheme)
{
    std::map<vec3d, uint, vert_compare> v_map;
    std::map<uint, uint> f_map;
    for(uint vid=0; vid<m.num_verts(); ++vid) v_map[m.vert(vid)] = vid;

    for(const auto &p : poly2scheme)
    {
        std::vector<vec3d>             verts;
        std::vector<std::vector<uint>> faces;
        std::vector<std::vector<uint>> polys;
        std::vector<std::vector<bool>> winding;

        vec3d poly_centroid = m.poly_centroid(p.first);
        SchemeInfo info = p.second;
        hex_transition_orient(verts, faces, polys, winding, info, poly_centroid);

        //merge vertices
        for(auto & v : verts)
        {
            if(v_map.find(v) == v_map.end())
            {
                uint fresh_vid = m.vert_add(v);
                v_map[v] = fresh_vid;
            }
        }

        //merge faces
        for(uint fid=0; fid<faces.size(); fid++)
        {
            auto f = faces[fid];
            for(uint &v : f) v = v_map[verts.at(v)];
            int test_id = m.face_id(f);
            if(test_id>=0)
            {
                f_map[fid] = test_id;
            }
            else
            {
                uint fresh_fid = m.face_add(f);
                f_map[fid] = fresh_fid;
            }
        }

        //merge_polys
        for(uint pid=0; pid<polys.size(); ++pid)
        {
            auto p = polys.at(pid);
            for(auto & fid : p) fid = f_map.at(fid);

            int test_id = m.poly_id(p);
            if(test_id==-1)
            {
                m.poly_add(p, winding[pid]);
            }
        }
    }
}

}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void hex_transition_install(const Polyhedralmesh<M,V,E,F,P> & m_in,
                            const std::vector<bool>         & transition_verts,
                                  Polyhedralmesh<M,V,E,F,P> & m_out)
{
    m_out = m_in;

    std::vector<uint> transition_verts_direction;
    get_transition_verts_direction(m_in, transition_verts, transition_verts_direction);

    //Each entry of these sets is a collection of transition vertices that compose a particular scheme
    std::set<uint> flats;
    std::set<std::vector<uint>> concaves;
    std::set<std::vector<uint>> corners;
    std::set<std::vector<uint>> convexes;
    std::unordered_set<uint>	not_flats; // Set containing all the transition verts that are not part of a flat transition

    //Schemes detection based on transition vertices configuration
    for(uint pid=0; pid<m_in.num_polys(); pid++)
    {
        std::vector<uint> scheme_vids;
        for(uint vid : m_in.poly_verts_id(pid))
        {
            if(transition_verts[vid]) scheme_vids.push_back(vid);
        }

        std::sort(scheme_vids.begin(), scheme_vids.end());
        bool flat_condition = scheme_vids.size() == 1;
        if(scheme_vids.size()==2){
            flat_condition = flat_condition || m_in.edge_id(scheme_vids[0], scheme_vids[1]) != -1;
        }
        if(flat_condition){
            for(uint sv : scheme_vids){
                if(not_flats.find(sv) == not_flats.end()) flats.insert(sv); //Flat
            }
        }
        else{
            if(scheme_vids.size() == 2){ //Concave edge candidate
                if(m_in.edge_id(scheme_vids[0], scheme_vids[1]) != -1) continue;
                //ensure that the transition verts are on the same poly face
                for(uint fid : m_in.poly_faces_id(pid)){
                    if(m_in.face_contains_vert(fid, scheme_vids[0]) &&
                       m_in.face_contains_vert(fid, scheme_vids[1])){

                        if(find_max_ref(m_in, scheme_vids[0]) == find_max_ref(m_in, scheme_vids[1])){
                            concaves.insert(scheme_vids);
                            for(uint vid : scheme_vids){
                                not_flats.insert(vid);
                                flats.erase(vid);
                            }

                            break;
                        }
                    }
                }
            }
            else if(scheme_vids.size() == 3){ //Concave vert
                corners.insert(scheme_vids);
                for(uint vid : scheme_vids){
                    not_flats.insert(vid);
                    flats.erase(vid);
                }
            }

        }
    }
    for(uint vid = 0; vid<m_in.num_verts(); vid++)
    {
        if(!transition_verts[vid])
        {
            std::vector<uint> adj_t_verts;
            for(uint adj_v : m_in.adj_v2v(vid))
            {
                if(transition_verts[adj_v]) adj_t_verts.push_back(adj_v);
            }
            if(adj_t_verts.size() == 3){
                int max = 0;
                for(uint v : adj_t_verts){
                    max = std::max(find_max_ref(m_in, v), max);
                }
                for(uint i=0; i<3;i++){
                    if(find_max_ref(m_in, adj_t_verts[i]) != max){
                        adj_t_verts.erase(std::next(adj_t_verts.begin(), i));
                        break;
                    }
                }
            }
            if(adj_t_verts.size() == 2 && transition_verts_direction[adj_t_verts[0]] != transition_verts_direction[adj_t_verts[1]])
            {
                //Convex candidate
                std::sort(adj_t_verts.begin(), adj_t_verts.end());
                if(concaves.find(adj_t_verts) == concaves.end())
                {
                    adj_t_verts.push_back(vid);
                    convexes.insert(adj_t_verts);
                }
            }
        }
    }

    std::unordered_map<uint, SchemeInfo> poly2scheme;
    for(const auto &corner : corners)   mark_concave_vert(m_out, corner, transition_verts_direction, poly2scheme);
    for(const auto &concave : concaves) mark_concave_edge(m_out, concave, transition_verts_direction, poly2scheme);
    for(const auto &convex : convexes)  mark_convex(m_out, convex, transition_verts_direction, poly2scheme);
    for(uint flat : flats)              mark_flat(m_out, flat, transition_verts_direction, poly2scheme);

    cut_flats(m_out, poly2scheme);
    merge_schemes_into_mesh(m_out, poly2scheme);

    std::vector<uint> polys_to_remove;
    for(auto p : poly2scheme)
    {
        polys_to_remove.push_back(p.first);
    }

    m_out.polys_remove(polys_to_remove);
    m_out.poly_fix_orientation();
}

}
