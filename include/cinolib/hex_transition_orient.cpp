/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2021: Marco Livesu                                              *
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
#include <cinolib/hex_transition_orient.h>
#include <cinolib/hex_transition_schemes.h>

namespace cinolib
{

namespace
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool eps_eq(const vec3d & a, const vec3d & b, double eps = 1e-6)
{
    return (a.dist(b) <= eps);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void reflect(std::vector<vec3d> & verts,
             const std::string  & axis)
{
    mat3d M = mat3d::DIAG(-1.0);
    if(axis.find('x') != std::string::npos) M._mat[0][0] = 1;
    if(axis.find('y') != std::string::npos) M._mat[1][1] = 1;
    if(axis.find('z') != std::string::npos) M._mat[2][2] = 1;
    for(auto & v : verts) v = M*v;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void rotate(std::vector<vec3d> & verts,
            const std::string  & axis,
            const double       & angle)
{
    vec3d vec(0,0,0);
    if(axis == "x") vec.x() = 1; else
    if(axis == "y") vec.y() = 1; else
    if(axis == "z") vec.z() = 1; else
    assert(false);

    mat3d R = mat3d::ROT_3D(vec, angle);
    for(auto & v : verts) v = R*v;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void try_reflections(std::vector<vec3d> & verts,
                     const SchemeInfo   & info,
                     const vec3d        & poly_centroid)
{
    std::vector<std::string> axes_to_try = { "x", "y", "z", "xy", "xz", "yz" };
    for(const auto & axis : axes_to_try)
    {
        auto tmp_verts = verts;
        reflect(tmp_verts, axis);
        AABB bbox(tmp_verts);
        for(auto &vert : tmp_verts)
        {
            vert -= bbox.center();
            vert += poly_centroid;
        }

        if(eps_eq(info.t_verts[0], tmp_verts.at(0)))
        {
            verts = tmp_verts;
            return;
        }
    }
    assert(false && "Failed to find the right reflection");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//This function is necessary to find the right rotation of CONVEX_2 transitions. Find a clever way to find the correct rotation without trying all the possible ones.
CINO_INLINE
void try_rotations(std::vector<vec3d> & verts,
                   const SchemeInfo   & info,
                   const vec3d        & poly_centroid)
{
    auto tmp_verts = verts;
    for(uint x=0; x<4; x++)
    {
        for(uint y=0; y<4; y++)
        {
            for(uint z=0; z<4; z++)
            {
                rotate(tmp_verts, "z", M_PI_2);
                AABB bbox(tmp_verts);
                for(auto &vert : tmp_verts)
                {
                    vert -= bbox.center();
                    vert += poly_centroid;
                }

                if((eps_eq(info.t_verts[0], tmp_verts.at(0)) && eps_eq(info.t_verts[1], tmp_verts.at(4))) ||
                   (eps_eq(info.t_verts[0], tmp_verts.at(4)) && eps_eq(info.t_verts[1], tmp_verts.at(0))))
                {
                    verts = tmp_verts;
                    return;
                }
            }
            rotate(tmp_verts, "y", M_PI_2);
        }
        rotate(tmp_verts, "x", M_PI_2);
    }
    assert(false && "Failed to find the right rotation");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void orient_flat(std::vector<vec3d>             & verts,
                 std::vector<std::vector<uint>> & faces,
                 std::vector<std::vector<uint>> & polys,
                 std::vector<std::vector<bool>> & winding,
                 SchemeInfo                     & info,
                 const vec3d                    & poly_centroid)
{
    if(info.type == HexTransition::FLAT)
    {
        verts.reserve(Flat::verts.size()/3);
        for(uint vid=0; vid<Flat::verts.size(); vid+=3)
        {
            verts.push_back(vec3d(Flat::verts[vid]-0.5, Flat::verts[vid+1]-0.5, Flat::verts[vid+2]-0.5));
        }
        polys   = Flat::polys;
        faces   = Flat::faces;
        winding = Flat::winding;
    }
    else
    {
        verts.reserve(Flat_Convex::verts.size()/3);
        for(uint vid=0; vid<Flat_Convex::verts.size(); vid+=3)
        {
            verts.push_back(vec3d(Flat_Convex::verts[vid]-0.5, Flat_Convex::verts[vid+1]-0.5, Flat_Convex::verts[vid+2]-0.5));
        }
        polys   = Flat_Convex::polys;
        faces   = Flat_Convex::faces;
        winding = Flat_Convex::winding;
    }

    if(info.scheme_type == SchemeType::FLAT_S)
    {
        switch(info.orientations[0])
        {
            case PLUS_Y:  break; //DEFAULT
            case PLUS_X:  rotate(verts, "z", -M_PI/2); break;
            case PLUS_Z:  rotate(verts, "x",  M_PI/2); break;
            case MINUS_X: rotate(verts, "z",  M_PI/2); break;
            case MINUS_Y: rotate(verts, "z",  M_PI);   break;
            case MINUS_Z: rotate(verts, "x", -M_PI/2); break;
        }
    }
    else if(info.scheme_type == SchemeType::CONC_S)
    {
        switch(info.orientations[0])
        {
            case PLUS_X:
            {
                rotate(verts, "z", -M_PI/2);
                if(info.orientations[1] == PLUS_Y)  break;
                if(info.orientations[1] == PLUS_Z)  rotate(verts, "x", -M_PI/2);
                if(info.orientations[1] == MINUS_Y) rotate(verts, "x",  M_PI);
                if(info.orientations[1] == MINUS_Z) rotate(verts, "x",  M_PI/2);
                break;
            }
            case PLUS_Y:
            {
                if(info.orientations[1] == PLUS_X ) break;
                if(info.orientations[1] == PLUS_Z ) rotate(verts, "y",  M_PI/2);
                if(info.orientations[1] == MINUS_X) rotate(verts, "y",  M_PI);
                if(info.orientations[1] == MINUS_Z) rotate(verts, "y", -M_PI/2);
                break;
            }
            case PLUS_Z:
            {
                rotate(verts, "x", M_PI/2);
                if(info.orientations[1] == PLUS_X ) break;
                if(info.orientations[1] == PLUS_Y ) rotate(verts, "z", -M_PI/2);
                if(info.orientations[1] == MINUS_X) rotate(verts, "z",  M_PI);
                if(info.orientations[1] == MINUS_Y) rotate(verts, "z",  M_PI/2);
                break;
            }
            case MINUS_X:
            {
                rotate(verts, "z", M_PI/2);
                if(info.orientations[1] == PLUS_Y ) break;
                if(info.orientations[1] == PLUS_Z ) rotate(verts, "x",  M_PI/2);
                if(info.orientations[1] == MINUS_Y) rotate(verts, "x",  M_PI);
                if(info.orientations[1] == MINUS_Z) rotate(verts, "x", -M_PI/2);
                break;
            }
            case MINUS_Y:
            {
                rotate(verts, "z", M_PI);
                if(info.orientations[1] == MINUS_X) break;
                if(info.orientations[1] == MINUS_Z) rotate(verts, "y",  M_PI/2);
                if(info.orientations[1] == PLUS_X ) rotate(verts, "y",  M_PI);
                if(info.orientations[1] == PLUS_Z ) rotate(verts, "y", -M_PI/2);
                break;
            }
            case MINUS_Z:
            {
                rotate(verts, "x", -M_PI/2);
                if(info.orientations[1] == MINUS_X) break;
                if(info.orientations[1] == MINUS_Y) rotate(verts, "z", -M_PI/2);
                if(info.orientations[1] == PLUS_X ) rotate(verts, "z",  M_PI);
                if(info.orientations[1] == PLUS_Y ) rotate(verts, "z",  M_PI/2);
                break;
            }
        }
    }
    else if(info.scheme_type == SchemeType::CORN_S)
    {
        std::sort(info.orientations.begin()+1, info.orientations.end());
        switch(info.orientations[0])
        {
            case PLUS_X:
            {
                rotate(verts, "z", -M_PI/2);
                reflect(verts, "xz");
                if(info.orientations[1] == PLUS_Y  && info.orientations[2] == PLUS_Z)    {} //DEFAULT;
                if(info.orientations[1] == PLUS_Y  && info.orientations[2] == MINUS_Z)   reflect(verts, "xy");
                if(info.orientations[1] == PLUS_Z  && info.orientations[2] == MINUS_Y)   reflect(verts, "xz");
                if(info.orientations[1] == MINUS_Y && info.orientations[2] == MINUS_Z) { reflect(verts, "xy"); reflect(verts, "xz"); }
                break;
            }
            case PLUS_Y:
            {
                rotate(verts, "y", M_PI/2);
                reflect(verts, "xy");

                if(info.orientations[1] == PLUS_X  && info.orientations[2] == PLUS_Z)   {} //DEFAULT
                if(info.orientations[1] == PLUS_X  && info.orientations[2] == MINUS_Z)  reflect(verts, "xy");
                if(info.orientations[1] == MINUS_X && info.orientations[2] == MINUS_Z)  reflect(verts, "y");
                if(info.orientations[1] == PLUS_Z  && info.orientations[2] == MINUS_X)  reflect(verts, "yz");
                break;
            }
            case PLUS_Z:
            {
                rotate(verts, "x", M_PI/2);
                reflect(verts, "xz");
                if(info.orientations[1] == PLUS_X  && info.orientations[2] == PLUS_Y)    {} //DEFAULT;
                if(info.orientations[1] == PLUS_Y  && info.orientations[2] == MINUS_X)   reflect(verts, "yz");
                if(info.orientations[1] == PLUS_X  && info.orientations[2] == MINUS_Y)   reflect(verts, "xz");
                if(info.orientations[1] == MINUS_X && info.orientations[2] == MINUS_Y) { reflect(verts, "yz"); reflect(verts, "xz"); }
                break;
            }
            case MINUS_X:
            {
                rotate(verts, "z", M_PI/2);
                if(info.orientations[1] == PLUS_Y  && info.orientations[2] == PLUS_Z)   {} //DEFAULT;
                if(info.orientations[1] == PLUS_Y  && info.orientations[2] == MINUS_Z)   reflect(verts, "xy");
                if(info.orientations[1] == PLUS_Z  && info.orientations[2] == MINUS_Y)   reflect(verts, "xz");;
                if(info.orientations[1] == MINUS_Y && info.orientations[2] == MINUS_Z) { reflect(verts, "yz"); reflect(verts, "xz"); }
                break;
            }
            case MINUS_Y:
            {
                rotate(verts, "y", M_PI/2);
                reflect(verts, "xy");
                reflect(verts, "xz");
                if(info.orientations[1] == PLUS_X  && info.orientations[2] == PLUS_Z)  {} //DEFAULT;
                if(info.orientations[1] == PLUS_X  && info.orientations[2] == MINUS_Z) reflect(verts, "xy");
                if(info.orientations[1] == MINUS_X && info.orientations[2] == MINUS_Z) reflect(verts, "y");
                if(info.orientations[1] == PLUS_Z  && info.orientations[2] == MINUS_X) reflect(verts, "yz");;
                break;
            }
            case MINUS_Z:
            {
                rotate(verts, "x", -M_PI/2);
                if(info.orientations[1] == PLUS_X  && info.orientations[2] == PLUS_Y)    {} //DEFAULT;
                if(info.orientations[1] == PLUS_Y  && info.orientations[2] == MINUS_X)   reflect(verts, "yz");;
                if(info.orientations[1] == PLUS_X  && info.orientations[2] == MINUS_Y)   reflect(verts, "xz");;
                if(info.orientations[1] == MINUS_X && info.orientations[2] == MINUS_Y) { reflect(verts, "yz"); reflect(verts, "xz");}
                break;
            }
        }
    }

    for(auto &vert : verts)
    {
        vert *= info.scale;
        vert += poly_centroid;
    }

    if(!eps_eq(info.t_verts[0], verts.at(0)))
    {
        try_reflections(verts, info, poly_centroid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void orient_convex(std::vector<vec3d>             & verts,
                   std::vector<std::vector<uint>> & faces,
                   std::vector<std::vector<uint>> & polys,
                   std::vector<std::vector<bool>> & winding,
                   SchemeInfo                     & info,
                   const vec3d                    & poly_centroid)
{
    if (info.type == HexTransition::CONVEX_1)
    {
        verts.reserve(Convex_1::verts.size()/3);
        for(uint vid=0; vid<Convex_1::verts.size(); vid+=3)
        {
            verts.push_back(vec3d(Convex_1::verts[vid]-0.5, Convex_1::verts[vid+1]-0.5, Convex_1::verts[vid+2]-0.5));
        }
        polys   = Convex_1::polys;
        faces   = Convex_1::faces;
        winding = Convex_1::winding;

        switch (info.orientations[0])
        {
            case PLUS_Y:  break; //DEFAULT
            case PLUS_X:  rotate(verts, "z", -M_PI/2); break;
            case PLUS_Z:  rotate(verts, "x",  M_PI/2); break;
            case MINUS_X: rotate(verts, "z",  M_PI/2); break;
            case MINUS_Y: rotate(verts, "z",  M_PI);   break;
            case MINUS_Z: rotate(verts, "x", -M_PI/2); break;
        }

        for(auto & v : verts)
        {
            v *= info.scale;
            v += poly_centroid;
        }
        if(!eps_eq(info.t_verts[0], verts.at(0)))
        {
            try_reflections(verts, info, poly_centroid);
        }
    }
    else if(info.type == HexTransition::CONVEX_2)
    {
        verts.reserve(Convex_2::verts.size()/3);
        for(uint vid=0; vid<Convex_2::verts.size(); vid+=3)
        {
            verts.push_back(vec3d(Convex_2::verts[vid]-0.5, Convex_2::verts[vid+1]-0.5, Convex_2::verts[vid+2]-0.5));
        }
        polys   = Convex_2::polys;
        faces   = Convex_2::faces;
        winding = Convex_2::winding;

        for(auto & v : verts)
        {
            v *= info.scale;
            v += poly_centroid;
        }

        if(!((eps_eq(info.t_verts[0], verts.at(0)) && eps_eq(info.t_verts[1], verts.at(4))) ||
             (eps_eq(info.t_verts[0], verts.at(4)) && eps_eq(info.t_verts[1], verts.at(0)))))
        {
            try_rotations(verts, info, poly_centroid);
        }
        return;
    }
    else if(info.type == HexTransition::CONVEX_3)
    {
        verts.reserve(Convex_3::verts.size()/3);
        for(uint vid=0; vid<Convex_3::verts.size(); vid+=3)
        {
            verts.push_back(vec3d(Convex_3::verts[vid]-0.5, Convex_3::verts[vid+1]-0.5, Convex_3::verts[vid+2]-0.5));
        }
        polys   = Convex_3::polys;
        faces   = Convex_3::faces;
        winding = Convex_3::winding;

        if(info.cuts[PLUS_X] && info.cuts[PLUS_Y]  && info.cuts[PLUS_Z])
        {
            rotate(verts, "y", M_PI); //10 11 12
        }
        else if(info.cuts[PLUS_X] && info.cuts[PLUS_Z] && info.cuts[MINUS_Y])
        {
            rotate(verts, "x", -M_PI_2); //10 12 14
            rotate(verts, "y",  M_PI);
        }
        else if(info.cuts[PLUS_Z] && info.cuts[MINUS_X] && info.cuts[MINUS_Y])
        {
            rotate(verts, "y", M_PI_2); //12 13 14
            rotate(verts, "z", M_PI_2);
        }
        else if(info.cuts[PLUS_X] && info.cuts[MINUS_Y] && info.cuts[MINUS_Z])
        {
            rotate(verts, "x", M_PI); //10 14 15
            rotate(verts, "y", M_PI);

        }
        else if(info.cuts[PLUS_X] && info.cuts[PLUS_Y] && info.cuts[MINUS_Z])
        {
            rotate(verts, "y", -M_PI/2); //10 11 15
        }
        else if(info.cuts[PLUS_Y] && info.cuts[PLUS_Z] && info.cuts[MINUS_X])
        {
            rotate(verts, "x", M_PI_2); //10 11 13
        }
        else if(info.cuts[MINUS_X] && info.cuts[MINUS_Y] && info.cuts[MINUS_Z])
        {
             rotate(verts, "z", M_PI_2); //13 14 15
        }
        else if(info.cuts[PLUS_Y] && info.cuts[MINUS_X] && info.cuts[MINUS_Z])
        {
            //rotate(scheme, "z", M_PI/2); //11 13 15
        }

        for(auto & v : verts)
        {
            v *= info.scale;
            v += poly_centroid;
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void orient_concave_edge(std::vector<vec3d>             & verts,
                         std::vector<std::vector<uint>> & faces,
                         std::vector<std::vector<uint>> & polys,
                         std::vector<std::vector<bool>> & winding,
                         SchemeInfo                     & info,
                         const vec3d                    & poly_centroid)
{
    if(info.type == HexTransition::EDGE)
    {
        verts.reserve(Edge::verts.size()/3);
        for(uint vid=0; vid<Edge::verts.size(); vid+=3)
        {
            verts.push_back(vec3d(Edge::verts[vid]-0.5, Edge::verts[vid+1]-0.5, Edge::verts[vid+2]-0.5));
        }
        polys   = Edge::polys;
        faces   = Edge::faces;
        winding = Edge::winding;
    }
    else
    {
        verts.reserve(Edge_WB::verts.size()/3);
        for(uint vid=0; vid<Edge_WB::verts.size(); vid+=3)
        {
            verts.push_back(vec3d(Edge_WB::verts[vid]-0.5, Edge_WB::verts[vid+1]-0.5, Edge_WB::verts[vid+2]-0.5));
        }
        polys   = Edge_WB::polys;
        faces   = Edge_WB::faces;
        winding = Edge_WB::winding;
    }

    reflect(verts, "xz");
    switch(info.orientations[0])
    {
        case PLUS_Y:                               reflect(verts, "xz"); break; //DEFAULT
        case PLUS_X:  rotate(verts, "z", -M_PI/2); reflect(verts, "yz"); break;
        case PLUS_Z:  rotate(verts, "x",  M_PI/2); reflect(verts, "xy"); break;
        case MINUS_X: rotate(verts, "z",  M_PI/2); reflect(verts, "yz"); break;
        case MINUS_Y: rotate(verts, "z",  M_PI);   reflect(verts, "xz"); break;
        case MINUS_Z: rotate(verts, "x", -M_PI/2); reflect(verts, "xy"); break;
    }

    for(auto & v : verts)
    {
        v *= info.scale;
        v += poly_centroid;
    }

    if(!eps_eq(info.t_verts[0], verts.at(0)))
    {
        try_reflections(verts, info, poly_centroid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void orient_concave_vert(std::vector<vec3d>             & verts,
                         std::vector<std::vector<uint>> & faces,
                         std::vector<std::vector<uint>> & polys,
                         std::vector<std::vector<bool>> & winding,
                         SchemeInfo                     & info,
                         const vec3d                    & poly_centroid)
{
    if(info.type == HexTransition::VERT_CENTER_WB_1)
    {
        verts.reserve(Vert_center_WB_1::verts.size()/3);
        for(uint vid=0; vid<Vert_center_WB_1::verts.size(); vid+=3)
        {
            verts.push_back(vec3d(Vert_center_WB_1::verts[vid]-0.5, Vert_center_WB_1::verts[vid+1]-0.5, Vert_center_WB_1::verts[vid+2]-0.5));
        }
        polys   = Vert_center_WB_1::polys;
        faces   = Vert_center_WB_1::faces;
        winding = Vert_center_WB_1::winding;
    }
    else if(info.type == HexTransition::VERT_CENTER_WB_2)
    {
        verts.reserve(Vert_center_WB_2::verts.size()/3);
        for(uint vid=0; vid<Vert_center_WB_2::verts.size(); vid+=3)
        {
            verts.push_back(vec3d(Vert_center_WB_2::verts[vid]-0.5, Vert_center_WB_2::verts[vid+1]-0.5, Vert_center_WB_2::verts[vid+2]-0.5));
        }
        polys   = Vert_center_WB_2::polys;
        faces   = Vert_center_WB_2::faces;
        winding = Vert_center_WB_2::winding;
    }
    else if(info.type == HexTransition::VERT_CENTER_WB_3)
    {
        verts.reserve(Vert_center_WB_3::verts.size()/3);
        for(uint vid=0; vid<Vert_center_WB_3::verts.size(); vid+=3)
        {
            verts.push_back(vec3d(Vert_center_WB_3::verts[vid]-0.5, Vert_center_WB_3::verts[vid+1]-0.5, Vert_center_WB_3::verts[vid+2]-0.5));
        }
        polys   = Vert_center_WB_3::polys;
        faces   = Vert_center_WB_3::faces;
        winding = Vert_center_WB_3::winding;
    }
    else
    {
        verts.reserve(Vert_center::verts.size()/3);
        for(uint vid=0; vid<Vert_center::verts.size(); vid+=3)
        {
            verts.push_back(vec3d(Vert_center::verts[vid]-0.5, Vert_center::verts[vid+1]-0.5, Vert_center::verts[vid+2]-0.5));
        }
        polys   = Vert_center::polys;
        faces   = Vert_center::faces;
        winding = Vert_center::winding;
   }

    std::sort(info.orientations.begin(), info.orientations.end());

    if(info.type == HexTransition::VERT_CENTER_WB_1)
    {
        if(info.cuts[PLUS_X])
        {
            rotate(verts, "z", M_PI/2);
            rotate(verts, "y", M_PI/2);
        }
        else if(info.cuts[PLUS_Y])
        {
            rotate(verts, "x", -M_PI/2);
            rotate(verts, "y", -M_PI/2);
        }
        else if(info.cuts[PLUS_Z])
        {
            //DEFAULT
        }
    }
    else if(info.type ==  HexTransition::VERT_CENTER_WB_2)
    {
        if(info.cuts[PLUS_X] && info.cuts[PLUS_Y])
        {
            rotate(verts, "x", M_PI/2);
            rotate(verts, "z", M_PI/2);
        }
        else if(info.cuts[PLUS_X] && info.cuts[PLUS_Z])
        {
            //DEFAULT
        }
        else if(info.cuts[PLUS_Y] && info.cuts[PLUS_Z])
        {
            rotate(verts, "x", -M_PI/2);
            rotate(verts, "y", -M_PI/2);
        }
    }

    //(info.orientations[0] == PLUS_X && info.orientations[1] == PLUS_Y && info.orientations[2] == PLUS_Z) //default
    if(info.orientations[0] == PLUS_X && info.orientations[1] == PLUS_Y && info.orientations[2] == MINUS_Z)
    {
        reflect(verts, "xy");
    }
    else if(info.orientations[0]==PLUS_Y  &&
            info.orientations[1]==MINUS_X &&
            info.orientations[2]==MINUS_Z)
    {
        reflect(verts, "y");
    }
    else if(info.orientations[0]==PLUS_Y  &&
            info.orientations[1]==PLUS_Z  &&
            info.orientations[2]==MINUS_X)
    {
        reflect(verts, "yz");
    }
    else if(info.orientations[0]==PLUS_X &&
            info.orientations[1]==PLUS_Z &&
            info.orientations[2]==MINUS_Y)
    {
        reflect(verts, "xz");
    }
    else if(info.orientations[0]==PLUS_X  &&
            info.orientations[1]==MINUS_Y &&
            info.orientations[2]==MINUS_Z)
    {
        reflect(verts, "xz");
        reflect(verts, "xy");
    }
    else if(info.orientations[0]==MINUS_X &&
            info.orientations[1]==MINUS_Y &&
            info.orientations[2]==MINUS_Z)
    {
        reflect(verts, "xz");
        reflect(verts, "y");
    }
    else if(info.orientations[0]==PLUS_Z  &&
            info.orientations[1]==MINUS_X &&
            info.orientations[2]==MINUS_Y )
    {
        reflect(verts, "xz");
        reflect(verts, "yz");
    }

    for(auto & v : verts)
    {
        v *= info.scale;
        v += poly_centroid;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void orient_concave_vert_side(std::vector<vec3d>             & verts,
                              std::vector<std::vector<uint>> & faces,
                              std::vector<std::vector<uint>> & polys,
                              std::vector<std::vector<bool>> & winding,
                              SchemeInfo                     & info,
                              const vec3d                    & poly_centroid)
{
    uint tv_idx = 0;
    if(info.type == HexTransition::VERT_SIDE_WB)
    {
        verts.reserve(Vert_side_WB::verts.size()/3);
        for(uint vid=0; vid<Vert_side_WB::verts.size(); vid+=3)
        {
            verts.push_back(vec3d(Vert_side_WB::verts[vid]-0.5, Vert_side_WB::verts[vid+1]-0.5, Vert_side_WB::verts[vid+2]-0.5));
        }
        polys   = Vert_side_WB::polys;
        faces   = Vert_side_WB::faces;
        winding = Vert_side_WB::winding;
        rotate(verts, "z", M_PI/2);
        rotate(verts, "y", M_PI/2);
    }
    else
    {
        verts.reserve(Vert_side::verts.size()/3);
        for(uint vid=0; vid<Vert_side::verts.size(); vid+=3)
        {
            verts.push_back(vec3d(Vert_side::verts[vid]-0.5, Vert_side::verts[vid+1]-0.5, Vert_side::verts[vid+2]-0.5));
        }
        polys   = Vert_side::polys;
        faces   = Vert_side::faces;
        winding = Vert_side::winding;
        tv_idx = 2;
    }

    std::sort(info.orientations.begin(), info.orientations.begin()+2);

    if(info.orientations[0] == PLUS_X && info.orientations[1] == PLUS_Y) //DEFAULT
    {
        rotate(verts, "y", M_PI/2);
        rotate(verts, "x", M_PI/2);

        vec3d tmp_v0 = verts.at(tv_idx);
        tmp_v0 *= info.scale;
        tmp_v0 += poly_centroid;
        if(!eps_eq(info.t_verts[0], tmp_v0)) reflect(verts, "xy");
    }
    else if(info.orientations[0] == PLUS_X && info.orientations[1] == PLUS_Z) // DEFAULT
    {
        if(info.orientations[2] == MINUS_Y) reflect(verts, "xz");
    }
    else if(info.orientations[0] == PLUS_X && info.orientations[1] == MINUS_Y)
    {
        rotate(verts, "y", M_PI/2);
        rotate(verts, "x", M_PI/2);
        reflect(verts, "xz");
        vec3d tmp_v0 = verts.at(tv_idx);
        tmp_v0 *= info.scale;
        tmp_v0 += poly_centroid;
        if(!eps_eq(info.t_verts[0], tmp_v0)) reflect(verts, "xy");
    }
    else if(info.orientations[0] == PLUS_X && info.orientations[1] == MINUS_Z)
    {
        reflect(verts, "xy");
        if(info.orientations[2] == MINUS_Y) reflect(verts, "xz");
    }
    else if(info.orientations[0] == PLUS_Y && info.orientations[1] == PLUS_Z) //DEFAULT
    {
        rotate(verts, "y", -M_PI/2);
        rotate(verts, "z", -M_PI/2);
        vec3d tmp_v0 = verts.at(tv_idx);
        tmp_v0 *= info.scale;
        tmp_v0 += poly_centroid;
        if(!eps_eq(info.t_verts[0], tmp_v0)) reflect(verts, "yz");
    }
    else if(info.orientations[0] == PLUS_Y && info.orientations[1] == MINUS_X)
    {
        rotate(verts, "y", M_PI/2);
        rotate(verts, "x", M_PI/2);
        reflect(verts, "yz");
        vec3d tmp_v0 = verts.at(tv_idx);
        tmp_v0 *= info.scale;
        tmp_v0 += poly_centroid;
        if(!eps_eq(info.t_verts[0], tmp_v0)) reflect(verts, "xy");
    }
    else if(info.orientations[0] == PLUS_Y && info.orientations[1] == MINUS_Z)
    {
        rotate(verts, "y", -M_PI/2);
        rotate(verts, "z", -M_PI/2);
        reflect(verts, "xy");
        vec3d tmp_v0 = verts.at(tv_idx);
        tmp_v0 *= info.scale;
        tmp_v0 += poly_centroid;
        if(!eps_eq(info.t_verts[0], tmp_v0)) reflect(verts, "yz");
    }
    else if(info.orientations[0] == PLUS_Z && info.orientations[1] == MINUS_X)
    {
         reflect(verts, "yz");
         if(info.orientations[2] == MINUS_Y) reflect(verts, "xz");
    }
    else if(info.orientations[0] == PLUS_Z && info.orientations[1] == MINUS_Y)
    {
        rotate(verts, "y", -M_PI/2);
        rotate(verts, "z", -M_PI/2);
        reflect(verts, "xz");
        vec3d tmp_v0 = verts.at(tv_idx);
        tmp_v0 *= info.scale;
        tmp_v0 += poly_centroid;
        if(!eps_eq(info.t_verts[0], tmp_v0)) reflect(verts, "yz");
    }
    else if(info.orientations[0] == MINUS_X && info.orientations[1] == MINUS_Y)
    {
        rotate(verts, "y", M_PI/2);
        rotate(verts, "x", M_PI/2);
        reflect(verts, "xz");
        reflect(verts, "yz");
        vec3d tmp_v0 = verts.at(tv_idx);
        tmp_v0 *= info.scale;
        tmp_v0 += poly_centroid;
        if(!eps_eq(info.t_verts[0], tmp_v0)) reflect(verts, "xy");
    }
    else if(info.orientations[0] == MINUS_X && info.orientations[1] == MINUS_Z)
    {
        reflect(verts, "y");
        if(info.orientations[2] == MINUS_Y) reflect(verts, "xz");
    }
    else if(info.orientations[0] == MINUS_Y && info.orientations[1] == MINUS_Z)
    {
        rotate(verts, "y", -M_PI/2);
        rotate(verts, "z", -M_PI/2);
        reflect(verts, "xz");
        reflect(verts, "xy");
        vec3d tmp_v0 = verts.at(tv_idx);
        tmp_v0 *= info.scale;
        tmp_v0 += poly_centroid;
        if(!eps_eq(info.t_verts[0], tmp_v0)) reflect(verts, "yz");
    }

    for(auto & v : verts)
    {
        v *= info.scale;
        v += poly_centroid;
    }
}

} // end anonymous namespace

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void hex_transition_orient(std::vector<vec3d>             & verts,
                           std::vector<std::vector<uint>> & faces,
                           std::vector<std::vector<uint>> & polys,
                           std::vector<std::vector<bool>> & winding,
                           SchemeInfo                     & info,
                           const vec3d                    & poly_centroid)
{
    switch(info.type)
    {
        case HexTransition::FLAT:
        case HexTransition::FLAT_CONVEX:
        {
            orient_flat(verts, faces, polys, winding, info, poly_centroid);
            break;
        }

        case HexTransition::CONVEX_1:
        case HexTransition::CONVEX_2:
        case HexTransition::CONVEX_3:
        {
            orient_convex(verts, faces, polys, winding, info, poly_centroid);
            break;
        }

        case HexTransition::EDGE:
        case HexTransition::EDGE_WB:
        {
            orient_concave_edge(verts, faces, polys, winding, info, poly_centroid);
            break;
        }
        case HexTransition::VERT_CENTER:
        case HexTransition::VERT_CENTER_WB_1:
        case HexTransition::VERT_CENTER_WB_2:
        case HexTransition::VERT_CENTER_WB_3:
        {
            orient_concave_vert(verts, faces, polys, winding, info, poly_centroid);
            break;
        }

        case HexTransition::VERT_SIDE:
        case HexTransition::VERT_SIDE_WB:
        {
            orient_concave_vert_side(verts, faces, polys, winding, info, poly_centroid);
            break;
        }
    }
}

}
