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
#include <cinolib/marching_tets.h>

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

CINO_INLINE
void make_triangle(const Tetmesh<>        & m,
                   const double             isovalue,
                   const uint               vids[],
                   const double             func[],
                   const uint               e[],
                   std::map<ipair,uint>   & e2v_map,
                   std::map<ipair,double> & split_info,
                   std::vector<double>    & coords,
                   std::vector<uint>      & tris,
                   std::vector<double>    & norm);


CINO_INLINE
void marching_tets(const Tetmesh<>        & m,
                   const double             isovalue,
                   std::vector<double>    & coords,
                   std::vector<uint>      & tris,
                   std::vector<double>    & norm,
                   std::map<ipair,double> & split_info)
{
    assert(split_info.empty());

    std::vector<u_char> c(m.num_cells(),0x0);

    for(uint cid=0; cid<m.num_cells(); ++cid)
    {
        double func[] =
        {
            m.vert_data(m.cell_vert_id(cid,0)).uvw[0],
            m.vert_data(m.cell_vert_id(cid,1)).uvw[0],
            m.vert_data(m.cell_vert_id(cid,2)).uvw[0],
            m.vert_data(m.cell_vert_id(cid,3)).uvw[0]
        };

        if (isovalue >= func[0]) c.at(cid) |= C_1000;
        if (isovalue >= func[1]) c.at(cid) |= C_0100;
        if (isovalue >= func[2]) c.at(cid) |= C_0010;
        if (isovalue >= func[3]) c.at(cid) |= C_0001;

        // In some degenerate cases (i.e. iso-surface touching an edge/face
        // exposed on the surface) one may get C_1111 and it is necessary to
        // invert the test sign to get it right (28th April @ EG2017)
        //
        if (c.at(cid) == C_1111)
        {
            c.at(cid) = 0x0;
            if (isovalue <= func[0]) c.at(cid) |= C_1000;
            if (isovalue <= func[1]) c.at(cid) |= C_0100;
            if (isovalue <= func[2]) c.at(cid) |= C_0010;
            if (isovalue <= func[3]) c.at(cid) |= C_0001;
        }
    }

    std::map<ipair,uint> e2v_map;

    for(uint cid=0; cid<m.num_cells(); ++cid)
    {
        uint vids[] =
        {
            m.cell_vert_id(cid,0),
            m.cell_vert_id(cid,1),
            m.cell_vert_id(cid,2),
            m.cell_vert_id(cid,3)
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
            m.cell_adjacent_through_face(cid,0),
            m.cell_adjacent_through_face(cid,1),
            m.cell_adjacent_through_face(cid,2),
            m.cell_adjacent_through_face(cid,3),
        };

        // Avoid triangle duplication and collapsed triangle generation when the iso-surface
        // passes EXACTLY on a vertex/edge/face shared between many tetrahedra.
        //
        switch (c.at(cid))
        {
            // iso-surface passes on a face : make sure only one tet (MUST BE the one with higher tid) triggers triangle generation...
            // Notice that if the adjacent tet is collapsed (C_1111), then it make sense to use the current one regardless the tid order
            case C_1110 : if (v_on_iso[0] && v_on_iso[1] && v_on_iso[2] && (int)cid < adj_tet[0] && c.at(adj_tet[0]) != C_1111) c.at(cid) = C_0000; break;
            case C_1101 : if (v_on_iso[0] && v_on_iso[1] && v_on_iso[3] && (int)cid < adj_tet[1] && c.at(adj_tet[1]) != C_1111) c.at(cid) = C_0000; break;
            case C_1011 : if (v_on_iso[0] && v_on_iso[2] && v_on_iso[3] && (int)cid < adj_tet[2] && c.at(adj_tet[2]) != C_1111) c.at(cid) = C_0000; break;
            case C_0111 : if (v_on_iso[1] && v_on_iso[2] && v_on_iso[3] && (int)cid < adj_tet[3] && c.at(adj_tet[3]) != C_1111) c.at(cid) = C_0000; break;

            // iso-surface passes on a edge : do nothing
            case C_0101 : if (v_on_iso[1] && v_on_iso[3]) c.at(cid) = C_0000; break;
            case C_1010 : if (v_on_iso[0] && v_on_iso[2]) c.at(cid) = C_0000; break;
            case C_0011 : if (v_on_iso[2] && v_on_iso[3]) c.at(cid) = C_0000; break;
            case C_1100 : if (v_on_iso[0] && v_on_iso[1]) c.at(cid) = C_0000; break;
            case C_1001 : if (v_on_iso[0] && v_on_iso[3]) c.at(cid) = C_0000; break;
            case C_0110 : if (v_on_iso[1] && v_on_iso[2]) c.at(cid) = C_0000; break;

            // iso-surface passes on a vertex : do nothing
            case C_1000 : if (v_on_iso[0]) c.at(cid) = C_0000; break;
            case C_0100 : if (v_on_iso[1]) c.at(cid) = C_0000; break;
            case C_0010 : if (v_on_iso[2]) c.at(cid) = C_0000; break;
            case C_0001 : if (v_on_iso[3]) c.at(cid) = C_0000; break;

            default : break;
        }

        // triangle generation
        switch (c.at(cid))
        {
            case C_1000 : { uint e [] = { 2, 0, 4 }; make_triangle(m, isovalue, vids, func, e , e2v_map, split_info, coords, tris, norm); break; }
            case C_0111 : { uint e [] = { 0, 2, 4 }; make_triangle(m, isovalue, vids, func, e , e2v_map, split_info, coords, tris, norm); break; }
            case C_1011 : { uint e [] = { 2, 1, 3 }; make_triangle(m, isovalue, vids, func, e , e2v_map, split_info, coords, tris, norm); break; }
            case C_0100 : { uint e [] = { 1, 2, 3 }; make_triangle(m, isovalue, vids, func, e , e2v_map, split_info, coords, tris, norm); break; }
            case C_1101 : { uint e [] = { 1, 0, 5 }; make_triangle(m, isovalue, vids, func, e , e2v_map, split_info, coords, tris, norm); break; }
            case C_0010 : { uint e [] = { 0, 1, 5 }; make_triangle(m, isovalue, vids, func, e , e2v_map, split_info, coords, tris, norm); break; }
            case C_0001 : { uint e [] = { 5, 3, 4 }; make_triangle(m, isovalue, vids, func, e , e2v_map, split_info, coords, tris, norm); break; }
            case C_1110 : { uint e [] = { 3, 5, 4 }; make_triangle(m, isovalue, vids, func, e , e2v_map, split_info, coords, tris, norm); break; }
            case C_0101 : { uint e0[] = { 5, 2, 4 }; make_triangle(m, isovalue, vids, func, e0, e2v_map, split_info, coords, tris, norm);
                            uint e1[] = { 2, 5, 1 }; make_triangle(m, isovalue, vids, func, e1, e2v_map, split_info, coords, tris, norm); break; }
            case C_1010 : { uint e0[] = { 2, 5, 4 }; make_triangle(m, isovalue, vids, func, e0, e2v_map, split_info, coords, tris, norm);
                            uint e1[] = { 5, 2, 1 }; make_triangle(m, isovalue, vids, func, e1, e2v_map, split_info, coords, tris, norm); break; }
            case C_0011 : { uint e0[] = { 3, 4, 1 }; make_triangle(m, isovalue, vids, func, e0, e2v_map, split_info, coords, tris, norm);
                            uint e1[] = { 1, 4, 0 }; make_triangle(m, isovalue, vids, func, e1, e2v_map, split_info, coords, tris, norm); break; }
            case C_1100 : { uint e0[] = { 4, 3, 1 }; make_triangle(m, isovalue, vids, func, e0, e2v_map, split_info, coords, tris, norm);
                            uint e1[] = { 4, 1, 0 }; make_triangle(m, isovalue, vids, func, e1, e2v_map, split_info, coords, tris, norm); break; }
            case C_1001 : { uint e0[] = { 3, 2, 0 }; make_triangle(m, isovalue, vids, func, e0, e2v_map, split_info, coords, tris, norm);
                            uint e1[] = { 5, 3, 0 }; make_triangle(m, isovalue, vids, func, e1, e2v_map, split_info, coords, tris, norm); break; }
            case C_0110 : { uint e0[] = { 2, 3, 0 }; make_triangle(m, isovalue, vids, func, e0, e2v_map, split_info, coords, tris, norm);
                            uint e1[] = { 3, 5, 0 }; make_triangle(m, isovalue, vids, func, e1, e2v_map, split_info, coords, tris, norm); break; }
            default : break;
        }
    }
}

CINO_INLINE
void make_triangle(const Tetmesh<>       & m,
                   const double            isovalue,
                   const uint              vids[],
                   const double            func[],
                   const uint               e[],
                   std::map<ipair,uint>   & e2v_map,
                   std::map<ipair,double> & split_info,
                   std::vector<double>    & coords,
                   std::vector<uint>      & tris,
                   std::vector<double>    & norm)
{
    assert(isovalue >= *std::min_element(func, func+4));
    assert(isovalue <= *std::max_element(func, func+4));

    vec3d v[3];

    uint fresh_vid = coords.size()/3;

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
            uint vid     = query->second;
            uint vid_ptr = vid * 3;
            tris.push_back(vid);
            v[i] = vec3d(coords[vid_ptr], coords[vid_ptr+1], coords[vid_ptr+2]);
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

            v[i] = (1.0 - alpha) * m.vert(v_a) + alpha * m.vert(v_b);

            coords.push_back(v[i].x());
            coords.push_back(v[i].y());
            coords.push_back(v[i].z());

            tris.push_back(fresh_vid);
            e2v_map[pair] = fresh_vid++;

            // useful to embed the iso-surface in the tessellation.
            // Tells me what mesh edges should be split
            if (alpha > 0 && alpha < 1)
            {
                split_info[pair] = (v_a == pair.first) ? 1.0-alpha : alpha;
            }
        }
    }

    vec3d u  = v[1] - v[0]; u.normalize();
    vec3d w  = v[2] - v[0]; w.normalize();
    vec3d n = u.cross(w);
    n.normalize();

    norm.push_back(n.x());
    norm.push_back(n.y());
    norm.push_back(n.z());
}

}
