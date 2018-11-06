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
*                                                                               *
*     Thomas Alderighi (thomas.alderighi@isti.cnr.it)                           *
*     http://vcg.isti.cnr.it/~alderighi/                                        *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Computer Science and Technologies (ISTI)                    *
*     Via Moruzzi, 1                                                            *
*     56124 Pisa,                                                               *
*     Italy                                                                     *
*********************************************************************************/
#include <cinolib/marching_tets.h>
#include <array>

namespace cinolib
{

enum // Look-up Table
{
    C_1111 = 0xF,
    C_0111 = 0x7,
    C_1011 = 0xB,
    C_0011 = 0x3,
    C_1101 = 0xD,
    C_0101 = 0x5,
    C_1001 = 0x9,
    C_0001 = 0x1,
    C_1110 = 0xE,
    C_0110 = 0x6,
    C_1010 = 0xA,
    C_0010 = 0x2,
    C_1100 = 0xC,
    C_0100 = 0x4,
    C_1000 = 0x8,
    C_0000 = 0x0
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void make_triangle(const Tetmesh<M,V,E,F,P> & m,
                   const double               isovalue,
                   const uint                 vids[],
                   const double               func[],
                   const std::array<uint,3> & e,
                   std::map<ipair,uint>     & e2v_map,
                   std::vector<vec3d>       & verts,
                   std::vector<uint>        & tris,
                   std::vector<vec3d>       & norms);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void marching_tets(const Tetmesh<M,V,E,F,P> & m,
                   const double               isovalue,
                   std::vector<vec3d>       & verts,
                   std::vector<uint>        & tris,
                   std::vector<vec3d>       & norms)
{
    /* FIXME: for all configurations where two verts >= isoval
     * and the other two are < isoval, this method will try to
     * make a quad (2 triangles).
     * Indeed, if one vertx has exactly isoval, the surface cuts
     * a triangle and not a quad inside the tet, and therefore
     * one of the two triangles will be degenerate.
     * To avoid confusion and excessive code specialization for corner
     * cases, maybe it is better to have three possible states for a
     * vertex (<,>,=). In this case each configuration will be 100% correct
    */

    std::vector<unsigned char> c(m.num_polys(),0x0);
    std::vector<bool> swapped(m.num_polys(), false);

    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        double func[] =
        {
            m.vert_data(m.poly_vert_id(pid,0)).uvw[0],
            m.vert_data(m.poly_vert_id(pid,1)).uvw[0],
            m.vert_data(m.poly_vert_id(pid,2)).uvw[0],
            m.vert_data(m.poly_vert_id(pid,3)).uvw[0]
        };

        if (isovalue >= func[0]) c.at(pid) |= C_1000;
        if (isovalue >= func[1]) c.at(pid) |= C_0100;
        if (isovalue >= func[2]) c.at(pid) |= C_0010;
        if (isovalue >= func[3]) c.at(pid) |= C_0001;

        /* If the isosurface does not intersect the tet,
         * one should get C_1111 using ">=", and C_0000
         * inverting to "<=".
         *
         * This does not happen if the isosurface passes
         * exhactly through one face. In this case one will
         * get C_1111 using ">=", and something like
         * C_0111 using "<=".
         *
         * Normally this does not create any trouble, as the
         * face-adjacent tet will trigger the generation of
         * that triangle. But if the tet is exposed on the
         * surface, then that triangle will be missing in the
         * final iso-surface.
         *
         * To avoid these missing triangles, whenever I get
         * a C_1111 I invert the sign, and assign to the tet
         * the configuration produced using "<="
        */
        if (c.at(pid) == C_1111)
        {
            swapped.at(pid) = true;
            c.at(pid) = 0x0;
            if (isovalue <= func[0]) c.at(pid) |= C_1000;
            if (isovalue <= func[1]) c.at(pid) |= C_0100;
            if (isovalue <= func[2]) c.at(pid) |= C_0010;
            if (isovalue <= func[3]) c.at(pid) |= C_0001;
        }
    }

    std::map<ipair,uint> e2v_map;

    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        uint vids[] =
        {
            m.poly_vert_id(pid,0),
            m.poly_vert_id(pid,1),
            m.poly_vert_id(pid,2),
            m.poly_vert_id(pid,3)
        };

        double func[] =
        {
            m.vert_data(vids[0]).uvw[0],
            m.vert_data(vids[1]).uvw[0],
            m.vert_data(vids[2]).uvw[0],
            m.vert_data(vids[3]).uvw[0]
        };

        bool v_on_iso[] =
        {
            func[0] == isovalue,
            func[1] == isovalue,
            func[2] == isovalue,
            func[3] == isovalue
        };

        int adj_tet[] // not uint because it may be -1 if there is no adjacent tet!
        {
            m.poly_adj_through_face(pid, m.poly_face_id(pid,0)),
            m.poly_adj_through_face(pid, m.poly_face_id(pid,1)),
            m.poly_adj_through_face(pid, m.poly_face_id(pid,2)),
            m.poly_adj_through_face(pid, m.poly_face_id(pid,3)),
        };

        // Avoid triangle duplication and collapsed triangle generation when the iso-surface
        // passes EXACTLY through a vertex/edge/face shared between many tetrahedra.
        //
        switch (c.at(pid))
        {
            // iso-surface passes on a face : make sure only one tet (MUST BE the one with higher id) triggers triangle generation...
            // Notice that if the adjacent tet is collapsed (C_1111), then it make sense to use the current one regardless the tid order
            case C_1110 : if (v_on_iso[0] && v_on_iso[1] && v_on_iso[2] && (int)pid < adj_tet[0] && c.at(adj_tet[0]) != C_1111) c.at(pid) = C_0000; break;
            case C_1101 : if (v_on_iso[0] && v_on_iso[1] && v_on_iso[3] && (int)pid < adj_tet[1] && c.at(adj_tet[1]) != C_1111) c.at(pid) = C_0000; break;
            case C_1011 : if (v_on_iso[0] && v_on_iso[2] && v_on_iso[3] && (int)pid < adj_tet[2] && c.at(adj_tet[2]) != C_1111) c.at(pid) = C_0000; break;
            case C_0111 : if (v_on_iso[1] && v_on_iso[2] && v_on_iso[3] && (int)pid < adj_tet[3] && c.at(adj_tet[3]) != C_1111) c.at(pid) = C_0000; break;

            // iso-surface passes on a edge : do nothing
            case C_0101 : if (v_on_iso[1] && v_on_iso[3]) c.at(pid) = C_0000; break;
            case C_1010 : if (v_on_iso[0] && v_on_iso[2]) c.at(pid) = C_0000; break;
            case C_0011 : if (v_on_iso[2] && v_on_iso[3]) c.at(pid) = C_0000; break;
            case C_1100 : if (v_on_iso[0] && v_on_iso[1]) c.at(pid) = C_0000; break;
            case C_1001 : if (v_on_iso[0] && v_on_iso[3]) c.at(pid) = C_0000; break;
            case C_0110 : if (v_on_iso[1] && v_on_iso[2]) c.at(pid) = C_0000; break;

            // iso-surface passes on a vertex : do nothing
            case C_1000 : if (v_on_iso[0]) c.at(pid) = C_0000; break;
            case C_0100 : if (v_on_iso[1]) c.at(pid) = C_0000; break;
            case C_0010 : if (v_on_iso[2]) c.at(pid) = C_0000; break;
            case C_0001 : if (v_on_iso[3]) c.at(pid) = C_0000; break;

            default : break;
        }

        // triangle generation
        switch (c.at(pid))
        {
            case C_1000 : { make_triangle(m, isovalue, vids, func, {2,0,4}, e2v_map, verts, tris, norms); break; }
            case C_0111 : { swapped.at(pid)?
                            make_triangle(m, isovalue, vids, func, {2,0,4}, e2v_map, verts, tris, norms) :
                            make_triangle(m, isovalue, vids, func, {0,2,4}, e2v_map, verts, tris, norms); break; }
            case C_1011 : { swapped.at(pid)?
                            make_triangle(m, isovalue, vids, func, {1,2,3}, e2v_map, verts, tris, norms) :
                            make_triangle(m, isovalue, vids, func, {2,1,3}, e2v_map, verts, tris, norms); break; }
            case C_0100 : { make_triangle(m, isovalue, vids, func, {1,2,3}, e2v_map, verts, tris, norms); break; }
            case C_1101 : { swapped.at(pid)?
                            make_triangle(m, isovalue, vids, func, {0,1,5}, e2v_map, verts, tris, norms) :
                            make_triangle(m, isovalue, vids, func, {1,0,5}, e2v_map, verts, tris, norms); break; }
            case C_0010 : { make_triangle(m, isovalue, vids, func, {0,1,5}, e2v_map, verts, tris, norms); break; }
            case C_0001 : { make_triangle(m, isovalue, vids, func, {5,3,4}, e2v_map, verts, tris, norms); break; }
            case C_1110 : { swapped.at(pid)?
                            make_triangle(m, isovalue, vids, func, {5,3,4}, e2v_map, verts, tris, norms) :
                            make_triangle(m, isovalue, vids, func, {3,5,4}, e2v_map, verts, tris, norms); break; }
            case C_0101 : { make_triangle(m, isovalue, vids, func, {5,2,4}, e2v_map, verts, tris, norms);
                            make_triangle(m, isovalue, vids, func, {2,5,1}, e2v_map, verts, tris, norms); break; }
            case C_1010 : { make_triangle(m, isovalue, vids, func, {2,5,4}, e2v_map, verts, tris, norms);
                            make_triangle(m, isovalue, vids, func, {5,2,1}, e2v_map, verts, tris, norms); break; }
            case C_0011 : { make_triangle(m, isovalue, vids, func, {3,4,1}, e2v_map, verts, tris, norms);
                            make_triangle(m, isovalue, vids, func, {1,4,0}, e2v_map, verts, tris, norms); break; }
            case C_1100 : { make_triangle(m, isovalue, vids, func, {4,3,1}, e2v_map, verts, tris, norms);
                            make_triangle(m, isovalue, vids, func, {4,1,0}, e2v_map, verts, tris, norms); break; }
            case C_1001 : { make_triangle(m, isovalue, vids, func, {3,2,0}, e2v_map, verts, tris, norms);
                            make_triangle(m, isovalue, vids, func, {5,3,0}, e2v_map, verts, tris, norms); break; }
            case C_0110 : { make_triangle(m, isovalue, vids, func, {2,3,0}, e2v_map, verts, tris, norms);
                            make_triangle(m, isovalue, vids, func, {3,5,0}, e2v_map, verts, tris, norms); break; }
            default : break;
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void make_triangle(const Tetmesh<M,V,E,F,P> & m,
                   const double               isovalue,
                   const uint                 vids[],
                   const double               func[],
                   const std::array<uint,3> & e,
                   std::map<ipair,uint>     & e2v_map,
                   std::vector<vec3d>       & verts,
                   std::vector<uint>        & tris,
                   std::vector<vec3d>       & norms)
{
    assert(isovalue >= *std::min_element(func, func+4));
    assert(isovalue <= *std::max_element(func, func+4));

    vec3d tri_verts[3];
    uint  fresh_vid = verts.size();

    for(uint i=0; i<3; ++i)
    {
        uint v_a = vids[TET_EDGES[e[i]][0]];
        uint v_b = vids[TET_EDGES[e[i]][1]];

        // avoid duplicated vertices. If an edge has already
        // been visited retrieve its id and coordinates
        //
        ipair pair  = unique_pair(v_a, v_b);
        auto  query = e2v_map.find(pair);

        if (query != e2v_map.end())
        {
            uint vid = query->second;
            tri_verts[i] = vec3d(verts.at(vid));
            tris.push_back(vid);
        }
        else
        {
            double f_a = func[TET_EDGES[e[i]][0]];
            double f_b = func[TET_EDGES[e[i]][1]];

            if (f_a < f_b)
            {
                std::swap(v_a, v_b);
                std::swap(f_a, f_b);
            }

            double alpha = (isovalue - f_a) / (f_b - f_a);

            tri_verts[i] = (1.0 - alpha) * m.vert(v_a) + alpha * m.vert(v_b);

            verts.push_back(tri_verts[i]);

            tris.push_back(fresh_vid);
            e2v_map[pair] = fresh_vid++;
        }
    }

    vec3d u  = tri_verts[1] - tri_verts[0]; u.normalize();
    vec3d w  = tri_verts[2] - tri_verts[0]; w.normalize();
    vec3d n = u.cross(w);
    n.normalize();
    norms.push_back(n);
}

}
