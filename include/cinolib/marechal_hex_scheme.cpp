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
#include <cinolib/marechal_hex_scheme.h>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void marechal(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                    Polyhedralmesh<M,V,E,F,P>         & m_out,
              const uint                                bot[5][5],
              const uint                                top[3][3])
{
    std::vector<vec3d> verts;
    std::vector<std::vector<uint>> faces;
    std::vector<std::vector<uint>> polys;
    marechal(m, bot, top, verts, faces, polys);

    for(auto v : verts) m_out.vert_add(v);
    for(auto f : faces) m_out.face_add(f);
    for(auto p : polys) m_out.poly_add(p, std::vector<bool>(p.size(),true)); // I am ignoring windind
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void marechal(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
              const uint                                bot[5][5],
              const uint                                top[3][3],
                    std::vector<vec3d>                & verts,
                    std::vector<std::vector<uint>>    & faces,
                    std::vector<std::vector<uint>>    & polys)
{
    // create inner points
    vec3d up = (m.vert(top[1][1]) - m.vert(bot[2][2]))*0.5;
    verts.push_back(m.vert(bot[0][2]) + up);
    verts.push_back(m.vert(bot[2][0]) + up);
    verts.push_back(m.vert(bot[2][1]) + up*0.6);
    verts.push_back(m.vert(bot[2][2]) + up*0.6);
    verts.push_back(m.vert(top[1][1]) - up);
    verts.push_back(m.vert(bot[2][3]) + up*0.6);
    verts.push_back(m.vert(bot[2][4]) + up);
    verts.push_back(m.vert(bot[4][2]) + up);
    //
    std::map<uint,uint> map;
    uint v_off = m.num_verts();
    map[bot[0][2]] = v_off;
    map[bot[2][0]] = v_off+1;
    map[bot[2][1]] = v_off+2;
    map[bot[2][2]] = v_off+3;
    map[top[1][1]] = v_off+4;
    map[bot[2][3]] = v_off+5;
    map[bot[2][4]] = v_off+6;
    map[bot[4][2]] = v_off+7;

    // floor
    // THESE ARE ASSUMED TO BE ALREADY IN THE INPUT MESH
    int b00 = m.face_id({bot[0][0], bot[0][1], bot[1][1], bot[1][0]}); assert(b00>=0);
    int b01 = m.face_id({bot[1][1], bot[0][1], bot[0][2], bot[1][2]}); assert(b01>=0);
    int b02 = m.face_id({bot[1][2], bot[0][2], bot[0][3], bot[1][3]}); assert(b02>=0);
    int b03 = m.face_id({bot[1][3], bot[0][3], bot[0][4], bot[1][4]}); assert(b03>=0);
    int b10 = m.face_id({bot[1][0], bot[1][1], bot[2][1], bot[2][0]}); assert(b10>=0);
    int b11 = m.face_id({bot[2][1], bot[1][1], bot[1][2], bot[2][2]}); assert(b11>=0);
    int b12 = m.face_id({bot[2][2], bot[1][2], bot[1][3], bot[2][3]}); assert(b12>=0);
    int b13 = m.face_id({bot[2][3], bot[1][3], bot[1][4], bot[2][4]}); assert(b13>=0);
    int b20 = m.face_id({bot[3][0], bot[2][0], bot[2][1], bot[3][1]}); assert(b20>=0);
    int b21 = m.face_id({bot[3][1], bot[2][1], bot[2][2], bot[3][2]}); assert(b21>=0);
    int b22 = m.face_id({bot[3][2], bot[2][2], bot[2][3], bot[3][3]}); assert(b22>=0);
    int b23 = m.face_id({bot[3][3], bot[2][3], bot[2][4], bot[3][4]}); assert(b23>=0);
    int b30 = m.face_id({bot[4][0], bot[3][0], bot[3][1], bot[4][1]}); assert(b30>=0);
    int b31 = m.face_id({bot[3][1], bot[3][2], bot[4][2], bot[4][1]}); assert(b31>=0);
    int b32 = m.face_id({bot[4][2], bot[3][2], bot[3][3], bot[4][3]}); assert(b32>=0);
    int b33 = m.face_id({bot[4][3], bot[3][3], bot[3][4], bot[4][4]}); assert(b33>=0);

    // ceiling
    // THESE ARE ASSUMED TO BE ALREADY IN THE INPUT MESH
    int t00 = m.face_id({top[0][0], top[1][0], top[1][1], top[0][1]}); assert(t00>=0);
    int t01 = m.face_id({top[0][1], top[0][2], top[1][2], top[1][1]}); assert(t01>=0);
    int t10 = m.face_id({top[1][0], top[1][1], top[2][1], top[2][0]}); assert(t10>=0);
    int t11 = m.face_id({top[2][1], top[2][2], top[1][2], top[1][1]}); assert(t11>=0);

    // triangular flaps attached to base (from face 0 to face 9)
    faces.push_back({bot[1][0], bot[2][0], map.at(bot[2][0])}); // f0
    faces.push_back({bot[2][0], bot[3][0], map.at(bot[2][0])}); // f1
    faces.push_back({bot[1][1], bot[2][1], map.at(bot[2][1])}); // f2
    faces.push_back({bot[2][1], bot[3][1], map.at(bot[2][1])}); // f3
    faces.push_back({bot[1][2], bot[2][2], map.at(bot[2][2])}); // f4
    faces.push_back({bot[3][2], bot[2][2], map.at(bot[2][2])}); // f5
    faces.push_back({bot[1][3], bot[2][3], map.at(bot[2][3])}); // f6
    faces.push_back({bot[3][3], bot[2][3], map.at(bot[2][3])}); // f7
    faces.push_back({bot[1][4], bot[2][4], map.at(bot[2][4])}); // f8
    faces.push_back({bot[3][4], bot[2][4], map.at(bot[2][4])}); // f9

    // rectangular flaps attached to base (from face 10 to face 13)
    faces.push_back({bot[2][0], bot[2][1], map.at(bot[2][1]), map.at(bot[2][0])}); // f10
    faces.push_back({bot[2][1], bot[2][2], map.at(bot[2][2]), map.at(bot[2][1])}); // f11
    faces.push_back({bot[2][2], bot[2][3], map.at(bot[2][3]), map.at(bot[2][2])}); // f12
    faces.push_back({bot[2][3], bot[2][4], map.at(bot[2][4]), map.at(bot[2][3])}); // f13

    // rectangular lids of the lower prism (from face 14 to face 21)
    faces.push_back({bot[1][0], bot[1][1], map.at(bot[2][1]), map.at(bot[2][0])}); // f14
    faces.push_back({bot[3][0], bot[3][1], map.at(bot[2][1]), map.at(bot[2][0])}); // f15
    faces.push_back({bot[1][1], bot[1][2], map.at(bot[2][2]), map.at(bot[2][1])}); // f16
    faces.push_back({bot[3][1], bot[3][2], map.at(bot[2][2]), map.at(bot[2][1])}); // f17
    faces.push_back({bot[1][2], bot[1][3], map.at(bot[2][3]), map.at(bot[2][2])}); // f18
    faces.push_back({bot[3][2], bot[3][3], map.at(bot[2][3]), map.at(bot[2][2])}); // f19
    faces.push_back({bot[1][3], bot[1][4], map.at(bot[2][4]), map.at(bot[2][3])}); // f20
    faces.push_back({bot[3][3], bot[3][4], map.at(bot[2][4]), map.at(bot[2][3])}); // f21

    // other internal faces (from face 22 to face 37)
    faces.push_back({       bot[4][2],         bot[4][3],  map.at(bot[4][2])}); // f22
    faces.push_back({       bot[0][2],         bot[0][3],  map.at(bot[0][2])}); // f23
    faces.push_back({       bot[4][2],         bot[4][1],  map.at(bot[4][2])}); // f24
    faces.push_back({       bot[0][2],         bot[0][1],  map.at(bot[0][2])}); // f25
    faces.push_back({map.at(bot[2][2]), map.at(bot[2][3]), map.at(top[1][1])}); // f26
    faces.push_back({map.at(bot[2][2]), map.at(bot[2][1]), map.at(top[1][1])}); // f27
    faces.push_back({map.at(bot[4][2]), map.at(top[1][1]),        top[1][1],         top[2][1]}); // f28
    faces.push_back({map.at(bot[0][2]), map.at(top[1][1]),        top[1][1],         top[0][1]}); // f29
    faces.push_back({map.at(top[1][1]), map.at(bot[2][3]), map.at(bot[2][4]),        top[1][2],         top[1][1]});  // f30
    faces.push_back({map.at(top[1][1]), map.at(bot[2][1]), map.at(bot[2][0]),        top[1][0],         top[1][1]});  // f31
    faces.push_back({map.at(bot[2][2]),        bot[3][2],         bot[4][2],  map.at(bot[4][2]), map.at(top[1][1])}); // f32
    faces.push_back({map.at(bot[2][3]),        bot[3][3],         bot[4][3],  map.at(bot[4][2]), map.at(top[1][1])}); // f33
    faces.push_back({map.at(bot[2][2]),        bot[1][2],         bot[0][2],  map.at(bot[0][2]), map.at(top[1][1])}); // f34
    faces.push_back({map.at(bot[2][3]),        bot[1][3],         bot[0][3],  map.at(bot[0][2]), map.at(top[1][1])}); // f35
    faces.push_back({map.at(bot[2][1]),        bot[3][1],         bot[4][1],  map.at(bot[4][2]), map.at(top[1][1])}); // f36
    faces.push_back({map.at(bot[2][1]),        bot[1][1],         bot[0][1],  map.at(bot[0][2]), map.at(top[1][1])}); // f37

    // lateral faces (excluded exposed faces of triangular prisms) (from face 38 to face 45)
    faces.push_back({bot[0][0], bot[0][1], map.at(bot[0][2]), top[0][1],        top[0][0]});  // f38
    faces.push_back({bot[0][4], bot[0][3], map.at(bot[0][2]), top[0][1],        top[0][2]});  // f39
    faces.push_back({bot[4][0], bot[4][1], map.at(bot[4][2]), top[2][1],        top[2][0]});  // f40
    faces.push_back({bot[4][4], bot[4][3], map.at(bot[4][2]), top[2][1],        top[2][2]});  // f41
    faces.push_back({bot[1][0], bot[0][0],        top[0][0],  top[1][0], map.at(bot[2][0])}); // f42
    faces.push_back({bot[1][4], bot[0][4],        top[0][2],  top[1][2], map.at(bot[2][4])}); // f43
    faces.push_back({bot[3][0], bot[4][0],        top[2][0],  top[1][0], map.at(bot[2][0])}); // f44
    faces.push_back({bot[3][4], bot[4][4],        top[2][2],  top[1][2], map.at(bot[2][4])}); // f45

    uint off = m.num_faces();

    // elements in the lower prism
    polys.push_back({ off+0, off+2, off+10, off+14, (uint)b10 });
    polys.push_back({ off+1, off+3, off+10, off+15, (uint)b20 });
    polys.push_back({ off+2, off+4, off+11, off+16, (uint)b11 });
    polys.push_back({ off+3, off+5, off+11, off+17, (uint)b21 });
    polys.push_back({ off+4, off+6, off+12, off+18, (uint)b12 });
    polys.push_back({ off+5, off+7, off+12, off+19, (uint)b22 });
    polys.push_back({ off+6, off+8, off+13, off+20, (uint)b13 });
    polys.push_back({ off+7, off+9, off+13, off+21, (uint)b23 });

    // custom elements
    polys.push_back({ off+19, off+22, off+32, off+33, off+26, (uint)b32 });
    polys.push_back({ off+18, off+26, off+23, off+34, off+35, (uint)b02 });
    polys.push_back({ off+17, off+32, off+24, off+36, off+27, (uint)b31 });
    polys.push_back({ off+16, off+27, off+34, off+25, off+37, (uint)b01 });
    polys.push_back({ off+21, off+33, off+45, off+41, off+28, off+30, (uint)t11, (uint)b33 });
    polys.push_back({ off+20, off+35, off+30, off+43, off+39, off+29, (uint)t01, (uint)b03 });
    polys.push_back({ off+15, off+36, off+28, off+44, off+40, off+31, (uint)t10, (uint)b30 });
    polys.push_back({ off+14, off+37, off+29, off+31, off+42, off+38, (uint)t00, (uint)b00 });
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void marechal_convex_element(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                                   Polyhedralmesh<M,V,E,F,P>         & m_out,                             
                             const uint                                edge[5],
                             const uint                                grid[3][4])
{
    std::vector<vec3d> verts;
    std::vector<std::vector<uint>> faces;
    std::vector<std::vector<uint>> polys;
    marechal_convex_element(m, edge, grid, verts, faces, polys);

    for(auto v : verts) m_out.vert_add(v);
    for(auto f : faces) m_out.face_add(f);
    for(auto p : polys) m_out.poly_add(p, std::vector<bool>(p.size(),true)); // I am ignoring windind
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void marechal_convex_element(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                             const uint                                edge[5],
                             const uint                                grid[3][4],                             
                                   std::vector<vec3d>                & verts,
                                   std::vector<std::vector<uint>>    & faces,
                                   std::vector<std::vector<uint>>    & polys)
{
    assert(edge[0] == grid[0][0]);
    assert(edge[2] == grid[1][0]);
    assert(edge[4] == grid[2][0]);

    // create inner points
    uint v11 = m.num_verts();
    uint v13 = v11 + 1;
    //
    vec3d u = (m.vert(grid[1][1]) - m.vert(grid[1][0]))*0.5;
    vec3d v = (m.vert(grid[1][3]) - m.vert(grid[1][0]))*0.5;
    verts.push_back(m.vert(grid[1][0]) + u);
    verts.push_back(m.vert(grid[1][0]) + v);

    // THESE ARE ASSUMED TO BE ALREADY IN THE INPUT MESH
    int f0   = m.face_id({grid[0][1], grid[0][2], grid[1][2], grid[1][1]}); assert(f0>=0);
    int f1   = m.face_id({grid[1][1], grid[1][2], grid[2][2], grid[2][1]}); assert(f1>=0);
    int f2   = m.face_id({grid[0][2], grid[0][3], grid[1][3], grid[1][2]}); assert(f2>=0);
    int f3   = m.face_id({grid[1][2], grid[1][3], grid[2][3], grid[2][2]}); assert(f3>=0);
    int ftop = m.face_id({grid[2][0], grid[2][1], grid[2][2], grid[2][3]}); assert(ftop>=0);
    int fbot = m.face_id({grid[0][0], grid[0][1], grid[0][2], grid[0][3]}); assert(fbot>=0);

    // faces of the two tetrahedra inside each cube
    faces.push_back({edge[1],    grid[1][0], v11 }); // f0
    faces.push_back({edge[1],    grid[1][0], v13 }); // f1
    faces.push_back({edge[3],    grid[1][0], v11 }); // f2
    faces.push_back({edge[3],    grid[1][0], v13 }); // f3
    faces.push_back({edge[1],    v11,        v13 }); // f4
    faces.push_back({edge[3],    v11,        v13 }); // f5
    faces.push_back({grid[1][0], v11,        v13 }); // f6

    // pentagonal faces
    faces.push_back({grid[1][3], grid[1][2], grid[1][1], v11,     v13 }); // f7
    faces.push_back({grid[1][1], grid[2][1], grid[2][0], edge[3], v11 }); // f8
    faces.push_back({grid[1][3], grid[2][3], grid[2][0], edge[3], v13 }); // f9
    faces.push_back({grid[1][1], grid[0][1], grid[0][0], edge[1], v11 }); // f10
    faces.push_back({grid[1][3], grid[0][3], grid[0][0], edge[1], v13 }); // f11

    uint off = m.num_faces();

    polys.push_back({ off+0, off+1, off+4,  off+6 });
    polys.push_back({ off+6, off+2, off+3,  off+5 });
    polys.push_back({ off+5, off+7, off+8,  off+9,  (uint)f1, (uint)f3, (uint)ftop});
    polys.push_back({ off+4, off+7, off+10, off+11, (uint)f0, (uint)f2, (uint)fbot});
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void marechal_concave_element(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                                    Polyhedralmesh<M,V,E,F,P>         & m_out,
                              const uint                                grid[5][5],
                              const uint                                edge[3])
{
    std::vector<vec3d> verts;
    std::vector<std::vector<uint>> faces;
    std::vector<std::vector<uint>> polys;
    marechal_concave_element(m, grid, edge, verts, faces, polys);

    for(auto v : verts) m_out.vert_add(v);
    for(auto f : faces) m_out.face_add(f);
    for(auto p : polys) m_out.poly_add(p, std::vector<bool>(p.size(),true)); // I am ignoring windind
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void marechal_concave_element(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                              const uint                                grid[5][5],
                              const uint                                edge[3],
                                    std::vector<vec3d>                & verts,
                                    std::vector<std::vector<uint>>    & faces,
                                    std::vector<std::vector<uint>>    & polys)
{
    // create inner points
    vec3d up0 = (m.vert(edge[0]) - m.vert(grid[0][0]))*0.5;
    vec3d up1 = (m.vert(edge[0]) - m.vert(grid[0][4]))*0.5;
    verts.push_back(m.vert(grid[2][0]) + up0*0.6);
    verts.push_back(m.vert(grid[2][1]) + up0*0.6);
    verts.push_back(m.vert(grid[2][3]) + up1*0.6);
    verts.push_back(m.vert(grid[2][4]) + up1*0.6);
    verts.push_back(m.vert(grid[2][0]) + up0);
    verts.push_back(m.vert(grid[2][4]) + up1);
    verts.push_back(m.vert(grid[0][0]) + up0*0.6);
    verts.push_back(m.vert(grid[4][0]) + up0*0.6);
    verts.push_back(m.vert(grid[0][4]) + up1*0.6);
    verts.push_back(m.vert(grid[4][4]) + up1*0.6);
    //
    std::map<uint,uint> map;
    uint v_off = m.num_verts();
    uint above_map_of_g20 = grid[1][0]; // just a unique number to identify the second vert above g[2][0]
    uint above_map_of_g24 = grid[1][4]; // just a unique number to identify the second vert above g[2][4]
    map[grid[2][0]] = v_off;
    map[grid[2][1]] = v_off+1;
    map[grid[2][3]] = v_off+2;
    map[grid[2][4]] = v_off+3;
    map[above_map_of_g20] = v_off+4;
    map[above_map_of_g24] = v_off+5;
    map[grid[0][0]] = v_off+6;
    map[grid[4][0]] = v_off+7;
    map[grid[0][4]] = v_off+8;
    map[grid[4][4]] = v_off+9;

    // THESE ARE ASSUMED TO BE ALREADY IN THE INPUT MESH
    int f00 = m.face_id({grid[0][0], grid[0][1], grid[1][1], grid[1][0]}); assert(f00>=0);
    int f01 = m.face_id({grid[1][1], grid[0][1], grid[0][2], grid[1][2]}); assert(f01>=0);
    int f02 = m.face_id({grid[1][2], grid[0][2], grid[0][3], grid[1][3]}); assert(f02>=0);
    int f03 = m.face_id({grid[1][3], grid[0][3], grid[0][4], grid[1][4]}); assert(f03>=0);
    int f10 = m.face_id({grid[1][0], grid[1][1], grid[2][1], grid[2][0]}); assert(f10>=0);
    int f11 = m.face_id({grid[2][1], grid[1][1], grid[1][2], grid[2][2]}); assert(f11>=0);
    int f12 = m.face_id({grid[2][2], grid[1][2], grid[1][3], grid[2][3]}); assert(f12>=0);
    int f13 = m.face_id({grid[2][3], grid[1][3], grid[1][4], grid[2][4]}); assert(f13>=0);
    int f20 = m.face_id({grid[3][0], grid[2][0], grid[2][1], grid[3][1]}); assert(f20>=0);
    int f21 = m.face_id({grid[3][1], grid[2][1], grid[2][2], grid[3][2]}); assert(f21>=0);
    int f22 = m.face_id({grid[3][2], grid[2][2], grid[2][3], grid[3][3]}); assert(f22>=0);
    int f23 = m.face_id({grid[3][3], grid[2][3], grid[2][4], grid[3][4]}); assert(f23>=0);
    int f30 = m.face_id({grid[4][0], grid[3][0], grid[3][1], grid[4][1]}); assert(f30>=0);
    int f31 = m.face_id({grid[3][1], grid[3][2], grid[4][2], grid[4][1]}); assert(f31>=0);
    int f32 = m.face_id({grid[4][2], grid[3][2], grid[3][3], grid[4][3]}); assert(f32>=0);
    int f33 = m.face_id({grid[4][3], grid[3][3], grid[3][4], grid[4][4]}); assert(f33>=0);

    // triangular flaps attached to background grid
    faces.push_back({grid[1][0], grid[2][0], map.at(grid[2][0])}); // f0
    faces.push_back({grid[2][0], grid[3][0], map.at(grid[2][0])}); // f1
    faces.push_back({grid[1][1], grid[2][1], map.at(grid[2][1])}); // f2
    faces.push_back({grid[2][1], grid[3][1], map.at(grid[2][1])}); // f3
    faces.push_back({grid[1][3], grid[2][3], map.at(grid[2][3])}); // f4
    faces.push_back({grid[3][3], grid[2][3], map.at(grid[2][3])}); // f5
    faces.push_back({grid[1][4], grid[2][4], map.at(grid[2][4])}); // f6
    faces.push_back({grid[3][4], grid[2][4], map.at(grid[2][4])}); // f7
    faces.push_back({grid[0][0], grid[0][1], map.at(grid[0][0])}); // f8
    faces.push_back({grid[4][0], grid[4][1], map.at(grid[4][0])}); // f9
    faces.push_back({grid[0][4], grid[0][3], map.at(grid[0][4])}); // f10
    faces.push_back({grid[4][4], grid[4][3], map.at(grid[4][4])}); // f11

    // rectangular flaps attached to base (from face 10 to face 13)
    faces.push_back({grid[2][0], grid[2][1], map.at(grid[2][1]), map.at(grid[2][0])}); // f12
    faces.push_back({grid[2][3], grid[2][4], map.at(grid[2][4]), map.at(grid[2][3])}); // f13

    // rectangular lids of the lower prism (from face 14 to face 21)
    faces.push_back({grid[1][0], grid[1][1], map.at(grid[2][1]), map.at(grid[2][0])}); // f14
    faces.push_back({grid[3][0], grid[3][1], map.at(grid[2][1]), map.at(grid[2][0])}); // f15
    faces.push_back({grid[1][3], grid[1][4], map.at(grid[2][4]), map.at(grid[2][3])}); // f16
    faces.push_back({grid[3][3], grid[3][4], map.at(grid[2][4]), map.at(grid[2][3])}); // f17

    // triangular flaps internal to the higher prisms
    faces.push_back({map.at(grid[2][0]), map.at(grid[2][1]), map.at(above_map_of_g20)}); // f18
    faces.push_back({map.at(grid[2][4]), map.at(grid[2][3]), map.at(above_map_of_g24)}); // f19

    // pentagonal lids in the lower prisms merged at the concavity
    faces.push_back({map.at(grid[2][3]), map.at(grid[2][1]), grid[2][1], grid[2][2], grid[2][3] }); // f20
    faces.push_back({map.at(grid[2][3]), map.at(grid[2][1]), grid[1][1], grid[1][2], grid[1][3] }); // f21
    faces.push_back({map.at(grid[2][3]), map.at(grid[2][1]), grid[3][1], grid[3][2], grid[3][3] }); // f22
    faces.push_back({map.at(grid[2][3]), map.at(grid[2][1]), map.at(above_map_of_g20), edge[1], map.at(above_map_of_g24) }); // f23

    // lids of the upper prisms
    faces.push_back({map.at(grid[0][0]), grid[0][1], grid[1][1], map.at(grid[2][1]), map.at(above_map_of_g20)}); // f24
    faces.push_back({map.at(grid[4][0]), grid[4][1], grid[3][1], map.at(grid[2][1]), map.at(above_map_of_g20)}); // f25
    faces.push_back({map.at(grid[0][4]), grid[0][3], grid[1][3], map.at(grid[2][3]), map.at(above_map_of_g24)}); // f26
    faces.push_back({map.at(grid[4][4]), grid[4][3], grid[3][3], map.at(grid[2][3]), map.at(above_map_of_g24)}); // f27

    // lateral faces (excluded triangular lids of the inner prisms)
    faces.push_back({grid[0][0], map.at(grid[0][0]), map.at(above_map_of_g20), map.at(grid[2][0]), grid[1][0] }); // f28
    faces.push_back({grid[4][0], map.at(grid[4][0]), map.at(above_map_of_g20), map.at(grid[2][0]), grid[3][0] }); // f29
    faces.push_back({grid[4][4], map.at(grid[4][4]), map.at(above_map_of_g24), map.at(grid[2][4]), grid[3][4] }); // f30
    faces.push_back({grid[0][4], map.at(grid[0][4]), map.at(above_map_of_g24), map.at(grid[2][4]), grid[1][4] }); // f31
    faces.push_back({map.at(above_map_of_g20), map.at(grid[0][0]), edge[0], edge[1]}); // f32
    faces.push_back({map.at(above_map_of_g20), map.at(grid[4][0]), edge[2], edge[1]}); // f33
    faces.push_back({map.at(above_map_of_g24), map.at(grid[0][4]), edge[0], edge[1]}); // f34
    faces.push_back({map.at(above_map_of_g24), map.at(grid[4][4]), edge[2], edge[1]}); // f35

    // top and bottom lids (excluded triangular lids of the inner prisms)
    faces.push_back({map.at(grid[0][0]), grid[0][1], grid[0][2], grid[0][3], map.at(grid[0][4]), edge[0]}); // f36
    faces.push_back({map.at(grid[4][0]), grid[4][1], grid[4][2], grid[4][3], map.at(grid[4][4]), edge[2]}); // f37

    uint off = m.num_faces();

    // lower prism
    polys.push_back({ (uint)f10, off+0, off+2, off+12, off+14 });
    polys.push_back({ (uint)f20, off+1, off+3, off+12, off+15 });
    polys.push_back({ (uint)f11, (uint)f12, off+2, off+4, off+20, off+21 });
    polys.push_back({ (uint)f21, (uint)f22, off+3, off+5, off+20, off+22 });
    polys.push_back({ (uint)f13, off+4, off+6, off+13, off+16 });
    polys.push_back({ (uint)f23, off+5, off+7, off+13, off+17 });

    // upper prism
    polys.push_back({ (uint)f00, off+28, off+14, off+18, off+ 8, off+24 });
    polys.push_back({ (uint)f30, off+29, off+15, off+18, off+ 9, off+25 });
    polys.push_back({ (uint)f03, off+31, off+16, off+19, off+10, off+26 });
    polys.push_back({ (uint)f33, off+30, off+17, off+19, off+11, off+27 });

    // lid elements
    polys.push_back( {(uint)f01, (uint)f02, off+24, off+26, off+32, off+34, off+36, off+23, off+21});
    polys.push_back( {(uint)f31, (uint)f32, off+25, off+27, off+33, off+35, off+37, off+23, off+22});
}

}
