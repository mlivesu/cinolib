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
#include <cinolib/map_to_tetrahedron.h>
#include <cinolib/standard_elements_tables.h>
#include <cinolib/dijkstra.h>
#include <cinolib/harmonic_map.h>
#include <cinolib/predicates.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void map_to_tetrahedron(const Trimesh<M,V,E,P> & m,
                              Trimesh<M,V,E,P> & m_out)
{
    m_out = m;
    std::vector<vec3d> verts;
    map_to_tetrahedron(m, verts);
    for(uint vid=0; vid<m_out.num_verts(); ++vid) m_out.vert(vid) = verts.at(vid);
    m_out.update_bbox();
    m_out.update_normals();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void map_to_tetrahedron(const Trimesh<M,V,E,P>   & m,
                              std::vector<vec3d> & verts) // verts mapped to tet
{
    assert(m.genus()==0);
    // try all triangles until you find a suitable one....
    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        // try all tri edges until you find a suitable one....
        for(uint eid : m.adj_p2e(pid))
        {
            uint vid0 = m.edge_vert_id(eid,0);
            uint vid1 = m.edge_vert_id(eid,1);
             int topp = m.poly_opposite_to(eid,pid);
            uint opp0 = m.vert_opposite_to(pid,vid0,vid1);
            uint opp1 = m.vert_opposite_to(topp,vid0,vid1);

            // if they form a tet with negative volume, use them to initialize the map
            // (note: the sign of orient3d is opposite to the volume and the jacobian)
            if(orient3d(m.poly_vert(pid,0),
                        m.poly_vert(pid,1),
                        m.poly_vert(pid,2),
                        m.vert(opp1))<0)
            {
                // find the path connecting opp0 and opp1 not passing through vid0 and vid1
                std::vector<uint> path;
                std::vector<bool> mask(m.num_verts(),false);
                mask.at(vid0) = true;
                mask.at(vid1) = true;
                dijkstra(m, opp0, opp1, mask, path);

                // assign canonical tet corners
                // v2 and v1 are flipped because in this case I want to map with a tet with negative volume!
                std::map<uint,vec3d> bcs;
                bcs[m.poly_vert_id(pid,0)] = REFERENCE_TET_VERTS[0];
                bcs[m.poly_vert_id(pid,1)] = REFERENCE_TET_VERTS[2];
                bcs[m.poly_vert_id(pid,2)] = REFERENCE_TET_VERTS[1];
                bcs[opp1]                  = REFERENCE_TET_VERTS[3];

                if(path.size()>2)
                {
                    // linearly interpolate the points along the path connecting opp0 and opp1
                    vec3d  dir  = bcs.at(opp1) - bcs.at(opp0);
                    double step = dir.norm()/(path.size()-1.0);
                    dir.normalize();
                    dir *= step;
                    for(uint i=1; i<path.size()-1; ++i)
                    {
                        bcs[path.at(i)] = bcs.at(opp0) + i*dir;
                    }
                }

                // map the rest of the vertices
                verts = harmonic_map_3d(m, bcs, 1, UNIFORM);
                return;
            }
        }
    }
    assert(false && "This is not supposed to happen! Perhaps normals point inwards?");
}

}
