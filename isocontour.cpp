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
bool is_into_interval(float v, float bound_0, float bound1)
{
    if (v >= bound_0 && v <= bound1) return true;
    if (v <= bound_0 && v >= bound1) return true;
    return false;
}

CINO_INLINE
bool orientation_check_passed(const vec3d & tn, const vec3d & e0, const vec3d & e1)
{
    return (tn.dot( e0.cross(e1) ) > 0);
}


CINO_INLINE
Isocontour::Isocontour()
{}


CINO_INLINE
Isocontour::Isocontour(Trimesh<> & m, float value, bool discard_boundary_edges)
: m_ptr(&m)
, iso_value(value)
, discard_edges_incident_to_boundary(discard_boundary_edges)
{
    make_iso_curve();
}

CINO_INLINE
void Isocontour::compute_edges2samples_map()
{
    for(uint eid=0; eid<m_ptr->num_edges(); ++eid)
    {
        uint  vid0 = m_ptr->edge_vert_id(eid, 0);
        uint  vid1 = m_ptr->edge_vert_id(eid, 1);
        float s0   = m_ptr->vert_data(vid0).uvw[0];
        float s1   = m_ptr->vert_data(vid1).uvw[0];

        // when the same border is valued 0 as seen from
        // one side and 1 as seen from the other side
        // a fake iso line may show up. So this is just
        // a stupid hack to avoid such behaviour. It is
        // OFF by default but can be switched on with the
        // proper call
        //
        // July 5th, 2017 : DEPRECATED (mainly because I am lazy and I don't want to re-introuce mesh borders in the new structure)
        //if (discard_edges_incident_to_boundary &&
        //   (m_ptr->vertex_is_border(vid0) ||
        //    m_ptr->vertex_is_border(vid1))) continue;

        if (is_into_interval(iso_value, s0, s1))
        {
            float alpha = fabs(iso_value - s0)/fabs(s0 - s1);

            edges2samples[eid] = (1.0 - alpha) * m_ptr->vert(vid0) + alpha * m_ptr->vert(vid1);
        }
    }
}


CINO_INLINE
void Isocontour::make_iso_curve()
{
    compute_edges2samples_map();

    std::set<uint> visited;

    for(std::map<uint,vec3d>::const_iterator it=edges2samples.begin(); it!=edges2samples.end(); ++it)
    {
        if (CONTAINS(visited, it->first)) continue;

        std::vector<vec3d> isocurve;
        std::vector<uint>  isocurve_edges;
        isocurve.push_back(it->second);
        isocurve_edges.push_back(it->first);

        std::map<uint,vec3d>::const_iterator first = it;
        std::map<uint,vec3d>::const_iterator curr  = it;
        std::map<uint,vec3d>::const_iterator last  = edges2samples.end();

        while(curr != edges2samples.end())
        {
            visited.insert(curr->first);

            std::map<uint,vec3d>::const_iterator next = next_edge(curr->first, visited);

            // if there exist a next edge
            // add a segment to the curve
            //
            if (next != edges2samples.end())
            {
                isocurve.push_back(next->second);
                isocurve_edges.push_back(next->first);
                last = next;
            }

            curr = next;
        }

        if (last != edges2samples.end())
        {
            // close the loop (if it is a loop)
            //
            if (m_ptr->edges_share_face(first->first, last->first))
            {
                isocurve.push_back(first->second);
                isocurve_edges.push_back(first->first);
            }

            curves_vertices.push_back(isocurve);
            curves_edges.push_back(isocurve_edges);

            uint cid = curves_edges.size() - 1;
            for(uint i=0; i<isocurve_edges.size(); ++i)
            {
                edges2curves[isocurve_edges[i]] = cid;
            }
        }
    }
}

CINO_INLINE
std::map<uint,vec3d>::const_iterator Isocontour::next_edge(uint eid, const std::set<uint> & visited) const
{
    std::vector<uint> vids;
    vids.push_back(m_ptr->edge_vert_id(eid,0));
    vids.push_back(m_ptr->edge_vert_id(eid,1));

    for(uint i=0; i<2; ++i)
    {
        uint vid = vids[i];

        std::vector<uint> eids = m_ptr->adj_v2e(vid);

        for(uint j=0; j<eids.size(); ++j)
        {
            std::map<uint,vec3d>::const_iterator query = edges2samples.find(eids[j]);

            if (query != edges2samples.end() && DOES_NOT_CONTAIN(visited, eids[j]))
            {
                // forces the curve to travel from one edge of a triangle
                // to another edge of the *same* triangle. serves to avoid
                // the creation of fake loops when more than two edges
                // incident to the same vertex are crossed by the iso-curve
                //
                if (m_ptr->edges_share_face(eid, query->first))
                {
                    return query;
                }
            }
        }
    }

    return edges2samples.end();
}

CINO_INLINE
uint Isocontour::num_curves() const
{
    return curves_vertices.size();
}

CINO_INLINE
std::vector<uint> Isocontour::tessellate(Trimesh<> & m) const
{
    // for each edge traversed by the isocurve, keep
    // a reference of the id of the new vertex added to
    // the mesh
    //
    std::map<uint,uint> edgesplit2newvid;
    std::vector<uint>   newly_generated_vertices;

    for(const auto & obj : edges2samples)
    {
        uint  eid    = obj.first;
        vec3d pos    = obj.second;
        uint  newvid = m.vert_add(pos);
        m.vert_data(newvid).uvw[0] = iso_value;
        newly_generated_vertices.push_back(newvid);
        edgesplit2newvid[eid] = newvid;
    }

    for(uint i=0; i<curves_vertices.size(); ++i)
    {
        if (curves_edges[i].front() != curves_edges[i].back())
        {
            std::cerr << "WARNING: " << i << "-th isocontour skipped because it is not a closed curve!" << std::endl;
            continue;
        }

        for(uint j=0; j<curves_edges[i].size()-1; ++j)
        {
            uint e0       = curves_edges[i][j];
            uint e1       = curves_edges[i][j+1];
            uint v0       = edgesplit2newvid.at(e0);
            uint v1       = edgesplit2newvid.at(e1);
            uint shared_v = m.vert_shared(e0,e1);
            int  fid      = m.face_shared(e0,e1); assert(fid!=-1);
            uint e0_opp   = m.vert_opposite_to(e0, shared_v);
            uint e1_opp   = m.vert_opposite_to(e1, shared_v);

            vec3d A = m.vert(shared_v);
            vec3d B = m.vert(v0);
            vec3d C = m.vert(v1);
            vec3d n = m.face_data(fid).normal;

                         m.face_set(fid, shared_v, v0, v1);
            uint newf0 = m.face_add(v0, e0_opp, e1_opp);
            uint newf1 = m.face_add(v0, e1_opp, v1);

            m.face_data(newf0) = m.face_data(fid);
            m.face_data(newf1) = m.face_data(fid);

            if (!orientation_check_passed(n, B-A, C-A))
            {
                m.face_flip_winding_order(fid);
                m.face_flip_winding_order(newf0);
                m.face_flip_winding_order(newf1);
            }
        }
    }

    return newly_generated_vertices;
}


CINO_INLINE
vec3d Isocontour::curve_centroid(uint cid) const
{
    vec3d c(0,0,0);
    for(uint i=0; i<curves_vertices[cid].size(); ++i)
    {
        c += curves_vertices[cid][i];
    }
    return c/= static_cast<double>(curves_vertices[cid].size());
}


CINO_INLINE
bool Isocontour::curve_is_closed(uint cid) const
{
    return (curves_edges[cid].front() == curves_edges[cid].back());
}


CINO_INLINE
bool Isocontour::curve_is_shorter_than(uint cid, uint size) const
{
    return (curves_edges[cid].size() < size);
}

CINO_INLINE
bool Isocontour::is_suspicious() const
{
    for(uint cid=0; cid<num_curves(); ++cid)
    {
        if (curve_is_shorter_than(cid, 5)) return true;
        if (!curve_is_closed(cid)) return true;
    }
    return false;
}

CINO_INLINE
void Isocontour::match(Isocontour & contour, std::set<std::pair<uint,uint> > & curve_matches) const
{
    // make sure we are on the same page (i.e. contours belong to the same mesh!)
    assert(m_ptr == contour.m_ptr);

    for(uint cid=0; cid<num_curves(); ++cid)
    {
        // start from each edge of each cc and flood the mesh, looking
        // for the first edge of the other isocontour
        //
        for(uint i=0; i<curves_edges[cid].size(); ++i)
        {
            uint eid = curves_edges[cid][i];

            // flood edges until you find an edge of the other isocurve
            //
            std::set<uint> visited;
            std::queue<uint> q;
            q.push(eid);

            while(!q.empty())
            {
                uint eid = q.front();
                q.pop();

                auto query = contour.edges2curves.find(eid);

                // if both curves share the edge, generate the correspondence right away
                //
                if (query != contour.edges2curves.end())
                {
                    curve_matches.insert(std::make_pair(cid, query->second));
                }
                else // otherwise do flood filling on the correct side of the curve
                {
                    uint  vid0 = m_ptr->edge_vert_id(eid, 0);
                    uint  vid1 = m_ptr->edge_vert_id(eid, 1);
                    float val0 = m_ptr->vert_data(vid0).uvw[0];
#ifndef NDEBUG
                    float val1 = m_ptr->vert_data(vid1).uvw[0];
#endif

                    std::vector<uint> edges;
                    if (is_into_interval(val0, this->iso_value, contour.iso_value))
                    {
                        edges = m_ptr->adj_v2e(vid0);
                    }
                    else
                    {
#ifndef NDEBUG
                        assert(is_into_interval(val1, this->iso_value, contour.iso_value));
#endif
                        edges = m_ptr->adj_v2e(vid1);
                    }

                    for(uint j=0; j<edges.size(); ++j)
                    {
                        if (DOES_NOT_CONTAIN(visited, edges[j]))
                        {
                            q.push(edges[j]);
                            visited.insert(edges[j]);
                        }
                    }
                }
            }
        }
    }
}

}
