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
#include "drawable_curve.h"
#include <cinolib/color.h>

namespace cinolib
{

CINO_INLINE
DrawableCurve::DrawableCurve() : Curve()
{}

CINO_INLINE
DrawableCurve::DrawableCurve(const std::vector<vec3d> & samples) : Curve(samples)
{}

CINO_INLINE
DrawableCurve::DrawableCurve(const Skel & skel, const int bone) : Curve(skel, bone)
{}


CINO_INLINE
void DrawableCurve::draw(const float scene_size) const
{
    double cylind_rad = scene_size*0.001;

    for(size_t i=1; i<samples().size(); ++i)
    {
        cylinder<vec3d>(samples()[i-1].pos, samples()[i].pos, cylind_rad, cylind_rad, Color::RED().rgba);
    }
}


CINO_INLINE
vec3d DrawableCurve::scene_center() const
{
    return bbox().center();
}


CINO_INLINE
float DrawableCurve::scene_radius() const
{
    return bbox().diag();
}



}
