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
#include <cinolib/earcut_mapping.h>
#include <cinolib/earcut.h>
#include <cinolib/polygon_topological_offsetting.h>

namespace cinolib
{

CINO_INLINE
void earcut_mapping(EarcutMapping_data & data)
{
    if(data.strictly_convex)
    {
        // due to convexity, the triangulation of poly_A will automatically apply to poly_B as well
        earcut(data.poly_A, data.tris, EarSorting::PRIORITIZED);
    }
    else
    {
        // poly_B is assumed to be star shaped, such that there exists a circle centered
        // at disk_center having radius disk_radius that is completely contained in its kernel

        // STEP1: apply topological offsetting, creating a convex inner polygon inside poly_B,
        // then apply the earcut mapping to the inner boundary of the offsetted polygons

        uint size = data.poly_A.size();

        // define offset points and tessellation in domain A
        std::vector<uint> vmap;
        polygon_topological_offsetting(data.poly_A, data.tris, vmap);

        // start by making a circle of inner vertices in the kernel of poly_B
        std::vector<vec3d> inner_chain;
        for(uint i=0; i<size; ++i)
        {
            vec3d v = data.poly_B.at(i) - data.disk_center;
            v.normalize();
            inner_chain.push_back(data.disk_center + v*data.disk_radius);
        }

        // then add extra vertices for those outer points having more than
        // one inner vertex in the chain
        for(uint i=0; i<size; ++i)
        {
            data.poly_B.push_back(inner_chain.at(i));

            if(vmap.at(i)>1)
            {
                vec3d v = inner_chain.at(i) - data.disk_center;
                vec3d u = inner_chain.at((i+1)%inner_chain.size()) - data.disk_center;
                v.normalize();
                u.normalize();
                vec3d axis = v.cross(u);
                axis.normalize();
                double alpha = u.angle_rad(v) * 0.5; // take opnly half of the available cone to avoid flips
                double step = alpha/static_cast<double>(vmap.at(i));
                double M[3][3];
                bake_rotation_matrix(axis, step, M);
                for(uint j=1;j<vmap.at(i); ++j)
                {
                    vec3d p = data.poly_B.back();
                    center_and_rotate(p, data.disk_center, M);
                    data.poly_B.push_back(p);
                }
            }
        }

        // eventually, mesh the inner polygon with earcut and append
        // the so generated triangles in the output triangle list
        std::vector<uint>  tmp_tris;
        std::vector<vec3d> tmp_poly(data.poly_A.begin()+size, data.poly_A.end());
        earcut(tmp_poly, tmp_tris, EarSorting::PRIORITIZED);
        for(uint vid : tmp_tris) data.tris.push_back(size+vid);
    }
}

}

