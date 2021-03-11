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
#include <cinolib/hex_transition.h>
#include <cinolib/hex_transition_schemes.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void hex_transition(Polyhedralmesh<M,V,E,F,P> & m,
                    const HexTransition         type,
                    const vec3d               & center,
                    const double                scale,
                    const int                   orientation)
{
    std::vector<double> verts;
    std::vector<std::vector<uint>> faces;
    std::vector<std::vector<uint>> polys;
    std::vector<std::vector<bool>> winding;
    hex_transition(type, verts, faces, polys, winding, center, scale, orientation);

    m = Polyhedralmesh<>(vec3d_from_serialized_xyz(verts), faces, polys, winding);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void hex_transition(const HexTransition                    type,
                          std::vector<double>            & verts,
                          std::vector<std::vector<uint>> & faces,
                          std::vector<std::vector<uint>> & polys,
                          std::vector<std::vector<bool>> & winding,
                    const vec3d                          & center,
                    const double                           scale,
                    const int                              orientation)
{
    switch(type)
    {
        case HexTransition::FLAT:
        {
            verts   = Flat::verts;
            faces   = Flat::faces;
            polys   = Flat::polys;
            winding = Flat::winding;
            break;
        }

        case HexTransition::FLAT_CONVEX:
        {
            verts   = Flat_Convex::verts;
            faces   = Flat_Convex::faces;
            polys   = Flat_Convex::polys;
            winding = Flat_Convex::winding;
            break;
        }

        case HexTransition::CONVEX_1:
        {
            verts   = Convex_1::verts;
            faces   = Convex_1::faces;
            polys   = Convex_1::polys;
            winding = Convex_1::winding;
            break;
        }

        case HexTransition::CONVEX_2:
        {
            verts   = Convex_2::verts;
            faces   = Convex_2::faces;
            polys   = Convex_2::polys;
            winding = Convex_2::winding;
            break;
        }

        case HexTransition::CONVEX_3:
        {
            verts   = Convex_3::verts;
            faces   = Convex_3::faces;
            polys   = Convex_3::polys;
            winding = Convex_3::winding;
            break;
        }

        case HexTransition::EDGE:
        {
            verts   = Edge::verts;
            faces   = Edge::faces;
            polys   = Edge::polys;
            winding = Edge::winding;
            break;
        }

        case HexTransition::VERT_CENTER:
        {
            verts   = Vert_center::verts;
            faces   = Vert_center::faces;
            polys   = Vert_center::polys;
            winding = Vert_center::winding;
            break;
        }

        case HexTransition::VERT_SIDE:
        {
            verts   = Vert_side::verts;
            faces   = Vert_side::faces;
            polys   = Vert_side::polys;
            winding = Vert_side::winding;
            break;
        }

        default: assert(false && "unknown scheme!");
    }

    uint nv = verts.size()/3;
    switch(orientation)
    {
        case PLUS_X:
        {
            for(uint vid=0; vid<nv; ++vid)
            {
                uint ptr = 3*vid;
                double & x = verts.at(ptr  );
                double & y = verts.at(ptr+1);
                std::swap(x,y);
            }
            break;
        }

        case PLUS_Y: break; // deaful orientation

        case PLUS_Z:
        {
            for(uint vid=0; vid<nv; ++vid)
            {
                uint ptr = 3*vid;
                double & y = verts.at(ptr+1);
                double & z = verts.at(ptr+2);
                std::swap(y,z);
            }
            break;
        }

        case MINUS_X:
        {
            for(uint vid=0; vid<nv; ++vid)
            {
                uint ptr = 3*vid;
                double & x = verts.at(ptr  );
                double & y = verts.at(ptr+1);
                std::swap(x,y);
                x = -x;
            }
            break;
        }

        case MINUS_Y:
        {
            for(uint vid=0; vid<nv; ++vid)
            {
                uint ptr = 3*vid;
                double & x = verts.at(ptr  );
                double & y = verts.at(ptr+1);
                x = -x;
                y = -y;
            }
            break;
        }

        case MINUS_Z:
        {
            for(uint vid=0; vid<nv; ++vid)
            {
                uint ptr = 3*vid;
                double & y = verts.at(ptr+1);
                double & z = verts.at(ptr+2);
                std::swap(y,z);
                z = -z;
            }
            break;
        }
    }

    // scale and center
    for(uint vid=0; vid<nv; ++vid)
    {
        uint ptr = 3*vid;
        double & x = verts.at(ptr  );
        double & y = verts.at(ptr+1);
        double & z = verts.at(ptr+2);
        //
        x *= scale;
        y *= scale;
        z *= scale;
        //
        x += center.x();
        y += center.y();
        z += center.z();
    }
}

}
