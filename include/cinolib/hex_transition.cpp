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
        case HexTransition::FLAT_FACE_4_TO_2:
        {
            verts   = Flat_Face_4_to_2::verts;
            faces   = Flat_Face_4_to_2::faces;
            polys   = Flat_Face_4_to_2::polys;
            winding = Flat_Face_4_to_2::winding;
            break;
        }

        case HexTransition::CONV_EDGE_4_TO_2:
        {
            verts   = Conv_Edge_4_to_2::verts;
            faces   = Conv_Edge_4_to_2::faces;
            polys   = Conv_Edge_4_to_2::polys;
            winding = Conv_Edge_4_to_2::winding;
            break;
        }

        case HexTransition::CONC_EDGE_4_TO_2:
        {
            verts   = Conc_Edge_4_to_2::verts;
            faces   = Conc_Edge_4_to_2::faces;
            polys   = Conc_Edge_4_to_2::polys;
            winding = Conc_Edge_4_to_2::winding;
            break;
        }

        case HexTransition::CONC_VERT_4_TO_2:
        {
            verts   = Conv_Vert_4_to_2::verts;
            faces   = Conv_Vert_4_to_2::faces;
            polys   = Conv_Vert_4_to_2::polys;
            winding = Conv_Vert_4_to_2::winding;
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
                x = -x;
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
                z = -z;
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
                y = -y;
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
                y = -y;
            }
            break;
        }
    }

    // scale and center
    for(uint vid=0; vid<nv; ++vid)
    {
        uint ptr = 3*vid;
        double & x = verts.at(ptr+1);
        double & y = verts.at(ptr+1);
        double & z = verts.at(ptr+2);
        //
        x *= scale;
        y *= scale;
        z *= scale;
        //
        x -= center.x();
        y -= center.y();
        z -= center.z();
    }
}

}
