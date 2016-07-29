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
#include <cinolib/isocontour.h>

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
Isocontour::Isocontour(Trimesh & m, float value, bool discard_boundary_edges)
: m_ptr(&m)
, iso_value(value)
, discard_edges_incident_to_boundary(discard_boundary_edges)
{
    make_iso_curve();
}

CINO_INLINE
void Isocontour::compute_edges2samples_map()
{
    for(int eid=0; eid<m_ptr->num_edges(); ++eid)
    {
        int   vid0 = m_ptr->edge_vertex_id(eid, 0);
        int   vid1 = m_ptr->edge_vertex_id(eid, 1);
        float s0   = m_ptr->vertex_u_text(vid0);
        float s1   = m_ptr->vertex_u_text(vid1);

        // when the same border is valued 0 as seen from
        // one side and 1 as seen from the other side
        // a fake iso line may show up. So this is just
        // a stupid hack to avoid such behaviour. It is
        // OFF by default but can be switched on with the
        // proper call
        //
        if (discard_edges_incident_to_boundary &&
           (m_ptr->vertex_is_border(vid0) ||
            m_ptr->vertex_is_border(vid1))) continue;

        if (is_into_interval(iso_value, s0, s1))
        {
            float alpha = fabs(iso_value - s0)/fabs(s0 - s1);

            edges2samples[eid] = (1.0 - alpha) * m_ptr->vertex(vid0) + alpha * m_ptr->vertex(vid1);
        }
    }
}


CINO_INLINE
void Isocontour::make_iso_curve()
{
    compute_edges2samples_map();

    std::set<int> visited;

    for(std::map<int,vec3d>::const_iterator it=edges2samples.begin(); it!=edges2samples.end(); ++it)
    {
        if (CONTAINS(visited, it->first)) continue;

        std::vector<vec3d> isocurve;
        std::vector<int>   isocurve_edges;
        isocurve.push_back(it->second);
        isocurve_edges.push_back(it->first);

        std::map<int,vec3d>::const_iterator first = it;
        std::map<int,vec3d>::const_iterator curr  = it;
        std::map<int,vec3d>::const_iterator last  = edges2samples.end();

        while (curr != edges2samples.end())
        {
            visited.insert(curr->first);

            std::map<int,vec3d>::const_iterator next = next_edge(curr->first, visited);

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
            if (m_ptr->edges_share_same_triangle(first->first, last->first))
            {
                isocurve.push_back(first->second);
                isocurve_edges.push_back(first->first);
            }

            curves_vertices.push_back(isocurve);
            curves_edges.push_back(isocurve_edges);

            int cid = curves_edges.size() - 1;
            for(size_t i=0; i<isocurve_edges.size(); ++i)
            {
                edges2curves[isocurve_edges[i]] = cid;
            }
        }
    }
}

CINO_INLINE
std::map<int,vec3d>::const_iterator Isocontour::next_edge(int eid, const std::set<int> & visited) const
{
    std::vector<int> vids;
    vids.push_back(m_ptr->edge_vertex_id(eid,0));
    vids.push_back(m_ptr->edge_vertex_id(eid,1));

    for(int i=0; i<2; ++i)
    {
        int vid = vids[i];

        std::vector<int> eids = m_ptr->adj_vtx2edg(vid);

        for(int j=0; j<(int)eids.size(); ++j)
        {
            std::map<int,vec3d>::const_iterator query = edges2samples.find(eids[j]);

            if (query != edges2samples.end() && DOES_NOT_CONTAIN(visited, eids[j]))
            {
                // forces the curve to travel from one edge of a triangle
                // to another edge of the *same* triangle. serves to avoid
                // the creation of fake loops when more than two edges
                // incident to the same vertex are crossed by the iso-curve
                //
                if (m_ptr->edges_share_same_triangle(eid, query->first))
                {
                    return query;
                }
            }
        }
    }

    return edges2samples.end();
}

CINO_INLINE
int Isocontour::num_curves() const
{
    return curves_vertices.size();
}

CINO_INLINE
std::vector<int> Isocontour::tessellate(Trimesh & m) const
{
    // for each edge traversed by the isocurve, keep
    // a reference of the id of the new vertex added to
    // the mesh
    //
    std::map<int,int> edgesplit2newvid;
    std::vector<int> newly_generated_vertices;

    for(std::map<int,vec3d>::const_iterator it=edges2samples.begin(); it!=edges2samples.end(); ++it)
    {
        int   eid    = it->first;
        vec3d pos    = it->second;
        int   newvid = m.add_vertex(pos, iso_value);
        edgesplit2newvid[eid] = newvid;
        newly_generated_vertices.push_back(newvid);
    }

    for(int i=0; i<(int)curves_vertices.size(); ++i)
    {
        // skip the curve if it is not closed
        //
        if (curves_edges[i].front() != curves_edges[i].back()) continue;

        for(int j=0; j<(int)curves_edges[i].size()-1; ++j)
        {
            int   e0       = curves_edges[i][j];
            int   e1       = curves_edges[i][j+1];
            int   v0       = edgesplit2newvid[e0];
            int   v1       = edgesplit2newvid[e1];
            int   shared_v = m.shared_vertex(e0,e1);
            int   tid      = m.shared_triangle(e0,e1);
            int   e0_opp   = m.vertex_opposite_to(e0, shared_v);
            int   e1_opp   = m.vertex_opposite_to(e1, shared_v);

            vec3d A = m.vertex(shared_v);
            vec3d B = m.vertex(v0);
            vec3d C = m.vertex(v1);
            vec3d n = m.triangle_normal(tid);

            int scalar = m.triangle_label(tid);

            if (orientation_check_passed(n, B-A, C-A))
            {
                m.set_triangle(tid, shared_v, v0, v1);
                m.add_triangle(v0, e0_opp, e1_opp, scalar);
                m.add_triangle(v0, e1_opp, v1, scalar);
            }
            else
            {
                m.set_triangle(tid, shared_v, v1, v0);
                m.add_triangle(v0, e1_opp, e0_opp, scalar);
                m.add_triangle(v0, v1,     e1_opp, scalar);
            }
        }
    }

    return newly_generated_vertices;
}


CINO_INLINE
vec3d Isocontour::curve_centroid(int cid) const
{
    vec3d c(0,0,0);
    for(int i=0; i<(int)curves_vertices[cid].size(); ++i)
    {
        c += curves_vertices[cid][i];
    }
    return c/=(double)curves_vertices[cid].size();
}


CINO_INLINE
bool Isocontour::curve_is_closed(int cid) const
{
    return (curves_edges[cid].front() == curves_edges[cid].back());
}


CINO_INLINE
bool Isocontour::curve_is_shorter_than(int cid, int size) const
{
    return ((int)curves_edges[cid].size() < size);
}

CINO_INLINE
bool Isocontour::is_suspicious() const
{
    for(int cid=0; cid<num_curves(); ++cid)
    {
        if (curve_is_shorter_than(cid, 5)) return true;
        if (!curve_is_closed(cid)) return true;
    }
    return false;
}

CINO_INLINE
void Isocontour::match(Isocontour & contour, std::set< std::pair<int,int> > & curve_matches) const
{
    // make sure we are on the same page (i.e. contours belong to the same mesh!)
    assert(m_ptr == contour.m_ptr);

    for(int cid=0; cid<num_curves(); ++cid)
    {
        // start from each edge of each cc and flood the mesh, looking
        // for the first edge of the other isocontour
        //
        for(size_t i=0; i<curves_edges[cid].size(); ++i)
        {
            int eid  = curves_edges[cid][i];

            // flood edges until you find an edge of the other isocurve
            //
            std::set<int> visited;
            std::queue<int> q;
            q.push(eid);

            while(!q.empty())
            {
                int eid = q.front();
                q.pop();

                std::map<int,int>::iterator query = contour.edges2curves.find(eid);

                // if both curves share the edge, generate the correspondence right away
                //
                if (query != contour.edges2curves.end())
                {
                    curve_matches.insert(std::make_pair(cid, query->second));
                }
                else // otherwise do flood filling on the correct side of the curve
                {
                    int   vid0 = m_ptr->edge_vertex_id(eid, 0);
                    int   vid1 = m_ptr->edge_vertex_id(eid, 1);
                    float val0 = m_ptr->vertex_u_text(vid0);
                    float val1 = m_ptr->vertex_u_text(vid1);

                    std::vector<int> edges;
                    if (is_into_interval(val0, this->iso_value, contour.iso_value))
                    {
                        edges = m_ptr->adj_vtx2edg(vid0);
                    }
                    else
                    {
                        assert(is_into_interval(val1, this->iso_value, contour.iso_value));
                        edges = m_ptr->adj_vtx2edg(vid1);
                    }

                    for(size_t j=0; j<edges.size(); ++j)
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
