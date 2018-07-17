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
#include <cinolib/bbox.h>

namespace cinolib
{

CINO_INLINE std::ostream & operator<<(std::ostream & in, const Bbox & bb)
{
    in << "\n[Bbox Info] MIN: " << bb.min << "\tMAX: " << bb.max <<
          "\nDiag: " << bb.diag() << "\tDeltaX: " << bb.delta_x() <<
          "\tDeltaY: " << bb.delta_y() << "\tDeltaZ: " << bb.delta_z() << "\n";
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Bbox::reset()
{
    min = vec3d( inf_double,  inf_double,  inf_double);
    max = vec3d(-inf_double, -inf_double, -inf_double);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d Bbox::center() const
{
    return (min + max) * 0.5;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Bbox::diag() const
{
    return (min - max).length();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Bbox::delta_x() const
{
    return (max.x() - min.x());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Bbox::delta_y() const
{
    return (max.y() - min.y());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Bbox::delta_z() const
{
    return (max.z() - min.z());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d Bbox::delta() const
{
    return (max - min);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Bbox::min_entry() const
{
    return std::min(min.min_entry(), max.min_entry());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Bbox::max_entry() const
{
    return  std::max(min.max_entry(), max.max_entry());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<vec3d> Bbox::corners(const double scaling_factor) const
{
    std::vector<vec3d> c =
    {
        min,
        min + vec3d(max.x(),       0,       0),
        min + vec3d(max.x(),       0, max.z()),
        min + vec3d(      0,       0, max.z()),
        min + vec3d(      0, max.y(),       0),
        min + vec3d(max.x(), max.y(),       0),
        min + vec3d(max.x(), max.y(), max.z()),
        min + vec3d(      0, max.y(), max.z()),
    };

    if(scaling_factor!=1.0)
    {
        for(vec3d & p : c) p -= center();
        for(vec3d & p : c) p *= scaling_factor;
        for(vec3d & p : c) p += center();
    }

    return c;
}

}
