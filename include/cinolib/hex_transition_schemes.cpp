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
#include <cinolib/hex_transition_schemes.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void hex_transition(Polyhedralmesh<M,V,E,F,P> & m,
                    const int                   type,
                    const vec3d               & center,
                    const double                scale,
                    const vec3d               & dir)
{
    std::vector<vec3d> verts;
    std::vector<std::vector<uint>> faces;
    std::vector<std::vector<uint>> polys;
    std::vector<std::vector<bool>> winding;

    switch(type)
    {
        case FLAT:
        {
            verts   = vec3d_from_serialized_xyz(HexTransitions::Flat::verts);
            faces   = HexTransitions::Flat::faces;
            polys   = HexTransitions::Flat::polys;
            winding = HexTransitions::Flat::winding;
            break;
        }

        case CONV:
        {
            verts   = vec3d_from_serialized_xyz(HexTransitions::ConvexEdge::verts);
            faces   = HexTransitions::ConvexEdge::faces;
            polys   = HexTransitions::ConvexEdge::polys;
            winding = HexTransitions::ConvexEdge::winding;
            break;
        }

        case CONC:
        {
            verts   = vec3d_from_serialized_xyz(HexTransitions::ConcaveEgde::verts);
            faces   = HexTransitions::ConcaveEgde::faces;
            polys   = HexTransitions::ConcaveEgde::polys;
            winding = HexTransitions::ConcaveEgde::winding;
            break;
        }

        case CORN:
        {
            verts   = vec3d_from_serialized_xyz(HexTransitions::ConcaveCorner::verts);
            faces   = HexTransitions::ConcaveCorner::faces;
            polys   = HexTransitions::ConcaveCorner::polys;
            winding = HexTransitions::ConcaveCorner::winding;
            break;
        }

        default: assert(false && "unknown scheme!");
    }

    m = Polyhedralmesh<M,V,E,F,P>(verts, faces, polys, winding);
    m.scale(scale);
    if(dir[0]==0 && dir[2]==0) // schemes are already oriented along the Y axis
    {
        if(dir[1]!=1) m.scale(dir[1]);
    }
    else
    {
        vec3d axis = dir.cross(vec3d(0,1,0));
        axis.normalize();
        double ang = dir.angle_rad(vec3d(0,1,0));
        m.rotate(axis, -ang);
    }
    m.translate(center - m.vert(0));
    m.update_bbox();
}

}
