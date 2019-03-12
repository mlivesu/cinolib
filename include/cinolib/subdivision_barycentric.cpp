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
#include <cinolib/subdivision_barycentric.h>
#include <unordered_map>

namespace cinolib
{

/* Implementation of barycentric subdivision for simplicial complexes of dimension 3.
 * See also: https://en.wikipedia.org/wiki/Barycentric_subdivision
*/
template<class M, class V, class E, class F, class P>
CINO_INLINE
void subdivision_barycentric(Tetmesh<M,V,E,F,P> & m)
{
    std::unordered_map<uint,uint> e_map; // edge midpoints
    std::unordered_map<uint,uint> f_map; // face centroids
    std::unordered_map<uint,uint> p_map; // poly centroids

    for(uint eid=0; eid<m.num_edges(); ++eid) e_map[eid] = m.vert_add(m.edge_sample_at(eid,0.5));
    for(uint fid=0; fid<m.num_faces(); ++fid) f_map[fid] = m.vert_add(m.face_centroid(fid));
    for(uint pid=0; pid<m.num_polys(); ++pid) p_map[pid] = m.vert_add(m.poly_centroid(pid));

    uint np = m.num_polys();
    for(uint pid=0; pid<np; ++pid)
    {
        // tet verts
        uint v[4] =
        {
            m.poly_vert_id(pid,0),
            m.poly_vert_id(pid,1),
            m.poly_vert_id(pid,2),
            m.poly_vert_id(pid,3),
        };

        // tet faces
        uint f[4][3] =
        {
            { v[TET_FACES[0][0]], v[TET_FACES[0][1]], v[TET_FACES[0][2]] },
            { v[TET_FACES[1][0]], v[TET_FACES[1][1]], v[TET_FACES[1][2]] },
            { v[TET_FACES[2][0]], v[TET_FACES[2][1]], v[TET_FACES[2][2]] },
            { v[TET_FACES[3][0]], v[TET_FACES[3][1]], v[TET_FACES[3][2]] },
        };

        // tet edges (sorted per face)
        uint e[4][3] =
        {
            { e_map.at(m.edge_id(f[0][0], f[0][1])),
              e_map.at(m.edge_id(f[0][1], f[0][2])),
              e_map.at(m.edge_id(f[0][2], f[0][0])) },

            { e_map.at(m.edge_id(f[1][0], f[1][1])),
              e_map.at(m.edge_id(f[1][1], f[1][2])),
              e_map.at(m.edge_id(f[1][2], f[1][0])) },

            { e_map.at(m.edge_id(f[2][0], f[2][1])),
              e_map.at(m.edge_id(f[2][1], f[2][2])),
              e_map.at(m.edge_id(f[2][2], f[2][0])) },

            { e_map.at(m.edge_id(f[3][0], f[3][1])),
              e_map.at(m.edge_id(f[3][1], f[3][2])),
              e_map.at(m.edge_id(f[3][2], f[3][0])) }
        };

        // face centroids
        uint fc[4] =
        {
            f_map.at(m.face_id({f[0][0], f[0][1], f[0][2]})),
            f_map.at(m.face_id({f[1][0], f[1][1], f[1][2]})),
            f_map.at(m.face_id({f[2][0], f[2][1], f[2][2]})),
            f_map.at(m.face_id({f[3][0], f[3][1], f[3][2]}))
        };

        // tet centroid
        uint c = p_map.at(pid);

        for(uint i=0; i<4; ++i)
        {
            // split i^th face
            m.poly_add({c, f[i][0], e[i][0], fc[i]});
            m.poly_add({c, e[i][0], f[i][1], fc[i]});
            m.poly_add({c, f[i][1], e[i][1], fc[i]});
            m.poly_add({c, e[i][1], f[i][2], fc[i]});
            m.poly_add({c, f[i][2], e[i][2], fc[i]});
            m.poly_add({c, e[i][2], f[i][0], fc[i]});
        }
    }

    // remove the old polys
    for(int pid=np-1; pid>=0; --pid) m.poly_remove(pid);
}


}
