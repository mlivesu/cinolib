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
#include <cinolib/cino_inline.h>
#include <cinolib/color.h>
#include <cinolib/gl/draw_sphere.h>
#include <cinolib/gl/draw_cylinder.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
DrawableIsocontour<M,V,E,P>::DrawableIsocontour()
: Isocontour<M,V,E,P>()
{
    color     = Color::RED();
    thickness = 1.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
DrawableIsocontour<M,V,E,P>::DrawableIsocontour(AbstractPolygonMesh<M,V,E,P> & m, double iso_value)
: Isocontour<M,V,E,P>(m, iso_value)
{
    color     = Color::RED();
    thickness = 1.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void DrawableIsocontour<M,V,E,P>::draw(const float scene_size) const
{
    float cylind_rad = scene_size * 0.002 * thickness;

    for(vec3d p : this->segs) sphere<vec3d>(p, cylind_rad, color.rgba);

    for(uint i=0; i<this->segs.size()/2; ++i)
    {
        cylinder<vec3d>(this->segs.at(2*i+0), this->segs.at(2*i+1), cylind_rad, cylind_rad, color.rgba);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
vec3d DrawableIsocontour<M,V,E,P>::scene_center() const
{
    return vec3d();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
float DrawableIsocontour<M,V,E,P>::scene_radius() const
{
    return 0.0;
}

}
