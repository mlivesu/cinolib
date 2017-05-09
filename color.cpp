/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2017                                                         *
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
#include "color.h"

namespace cinolib
{

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Color & c)
{
    in << "RGBA: " << c.r << " " << c.g << " " << c.b << " " << c.a << "\n";
    return in;
}


CINO_INLINE
Color Color::rgb_from_quality(float q)
{
    if (q < 0) return Color(1,0,0);

    if (q <= 0.5)
    {
        q *= 2.0;
        return Color(WHITE().r * q + RED().r * (1.0 - q),
                     WHITE().g * q + RED().g * (1.0 - q),
                     WHITE().b * q + RED().b * (1.0 - q));
    }

    if (q <= 1.0)
    {
        q = 2.0 * q - 1.0;
        return Color(BLUE().r * q + WHITE().r * (1.0 - q),
                     BLUE().g * q + WHITE().g * (1.0 - q),
                     BLUE().b * q + WHITE().b * (1.0 - q));
    }

    assert(false);
}

}
