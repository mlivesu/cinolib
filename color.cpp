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
