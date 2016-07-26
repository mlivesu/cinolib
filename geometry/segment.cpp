/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#include "segment.h"

namespace cinolib
{

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Segment & s)
{
    in << s.first << "\t" << s.second << "\n";
    return in;
}


CINO_INLINE
Segment::Segment(const vec3d & P0, const vec3d & P1)
{
    first  = P0;
    second = P1;
}


CINO_INLINE
double Segment::operator[](const vec3d & p) const
{
    return dist_to_point(p);
}


CINO_INLINE
double Segment::dist_to_point(const vec3d & p) const
{
    vec3d v = second - first;
    vec3d w = p  - first;

    float cos_wv = w.dot(v);
    float cos_vv = v.dot(v);

    if (cos_wv <= 0.0)    return (p-first).length();
    if (cos_vv <= cos_wv) return (p-second).length();

    float b  = cos_wv / cos_vv;
    vec3d Pb = first + v*b;
    return (p-Pb).length();
}


}
