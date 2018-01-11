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
#ifndef CINO_DRAW_ARROW_H
#define CINO_DRAW_ARROW_H

#include <cinolib/cino_inline.h>
#include <cinolib/gl/draw_cylinder.h>

namespace cinolib
{

template <typename vec3>
CINO_INLINE
void arrow(const vec3  & base,
           const vec3  & tip,
           float         radius,
           const float * color)
{
    vec3 tip_base = 0.3 * tip + 0.7 * base;

    cylinder<vec3>(tip_base, tip, radius, 0.0, color);
    cylinder<vec3>(base, tip_base, radius * 0.2,  radius * 0.2, color);
}

}

#endif // CINO_DRAW_ARROW_H
