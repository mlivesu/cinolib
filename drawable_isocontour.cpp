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
#include <cinolib/drawable_isocontour.h>
#include <cinolib/colors.h>
#include <cinolib/gl/draw_sphere.h>
#include <cinolib/gl/draw_cylinder.h>

namespace cinolib
{

CINO_INLINE
DrawableIsocontour::DrawableIsocontour()
{
    type = CURVE;
}

CINO_INLINE
DrawableIsocontour::DrawableIsocontour(Trimesh<> & m, float value) : Isocontour(m, value)
{
    type = CURVE;

    for(int i=0; i<3; ++i)
    {
        cylind_rgb[i] = YELLOW[i];
        centre_rgb[i] = BLUE[i];
        sample_rgb[i] = RED[i];
    }
}


CINO_INLINE
void DrawableIsocontour::draw(const float scene_size) const
{
    float sample_rad = /*m_ptr->bbox().diag()*/scene_size*0.004;
    float cylind_rad = /*m_ptr->bbox().diag()*/scene_size*0.002;

    for(int i=0; i<(int)curves_vertices.size(); ++i)
    {
        for(int j=0; j<(int)curves_vertices[i].size()-1; ++j)
        {
            cylinder<vec3d>(curves_vertices[i][j], curves_vertices[i][j+1], cylind_rad, cylind_rad, cylind_rgb);
        }
        sphere<vec3d>(curve_centroid(i), sample_rad, centre_rgb);
    }
}

}
