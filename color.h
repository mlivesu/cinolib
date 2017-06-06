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
#ifndef CINO_COLOR_H
#define CINO_COLOR_H

#include <cinolib/cinolib.h>

namespace cinolib
{

class Color
{
    public:

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        static Color RED()     { return Color(1,0,0); }
        static Color GREEN()   { return Color(0,1,0); }
        static Color BLUE()    { return Color(0,0,1); }
        static Color YELLOW()  { return Color(1,1,0); }
        static Color MAGENTA() { return Color(1,0,1); }
        static Color CYAN()    { return Color(0,1,1); }
        static Color BLACK()   { return Color(0,0,0); }
        static Color WHITE()   { return Color(1,1,1); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Color(const float r=1.0,
              const float g=1.0,
              const float b=1.0,
              const float a=1.0)
        : r(r), g(g), b(b), a(a) {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        static Color rgb_from_quality(float q);
        static Color scatter(uint n_colors, uint pos); // TODO: steal Color::scatter from VCG!!!

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        union
        {
            struct
            {
                float r, g, b, a;
            };
            float rgba[4];
        };
};

CINO_INLINE std::ostream & operator<<(std::ostream & in, const Color & c);

}

#ifndef  CINO_STATIC_LIB
#include "color.cpp"
#endif

#endif // CINO_COLOR_H
