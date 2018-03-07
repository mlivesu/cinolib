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

#include <cinolib/cino_inline.h>
#include <stdint.h>
#include <iostream>

namespace cinolib
{

class Color
{
    public:

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        static Color RED()     { return Color( 1, 0, 0); }
        static Color GREEN()   { return Color( 0, 1, 0); }
        static Color BLUE()    { return Color( 0, 0, 1); }
        static Color YELLOW()  { return Color( 1, 1, 0); }
        static Color MAGENTA() { return Color( 1, 0, 1); }
        static Color CYAN()    { return Color( 0, 1, 1); }
        static Color BLACK()   { return Color( 0, 0, 0); }
        static Color WHITE()   { return Color( 1, 1, 1); }
        static Color GRAY()    { return Color(.5,.5,.5); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        static Color PASTEL_YELLOW()  { return Color(254.0/255.0, 229.0/255.0, 157.0/255.0); }
        static Color PASTEL_MAGENTA() { return Color(252.0/255.0,  89.0/255.0, 148.0/255.0); }
        static Color PASTEL_GREEN()   { return Color(198.0/255.0, 223.0/255.0, 182.0/255.0); }
        static Color PASTEL_CYAN()    { return Color( 77.0/255.0, 193.0/255.0, 198.0/255.0); }
        static Color PASTEL_VIOLET()  { return Color(124.0/255.0, 158.0/255.0, 251.0/255.0); }
        static Color PASTEL_PINK()    { return Color(182.0/255.0, 200.0/255.0, 230.0/255.0); }
        static Color PASTEL_RED()     { return Color(253.0/255.0, 104.0/255.0, 118.0/255.0); }
        static Color PASTEL_ORANGE()  { return Color(253.0/255.0, 135.0/255.0,  86.0/255.0); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit Color(const float r=1.0,
                       const float g=1.0,
                       const float b=1.0,
                       const float a=1.0) : r(r), g(g), b(b), a(a) {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

              bool    operator==(const Color & c) const;
              bool    operator!=(const Color & c) const;
              bool    operator< (const Color & c) const;
              float & operator[](const uint    i);
        const float & operator[](const uint    i) const;
              Color & operator*=(const double  d);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint8_t r_uchar() const { return static_cast<uint8_t>(r*255.0); }
        uint8_t g_uchar() const { return static_cast<uint8_t>(g*255.0); }
        uint8_t b_uchar() const { return static_cast<uint8_t>(b*255.0); }
        uint8_t a_uchar() const { return static_cast<uint8_t>(a*255.0); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        static Color scatter(uint range, uint value, float sat=.5f, float val=.85f);
        static Color hsv_ramp(uint range, uint value);
        static Color quality2rgb(float q);
        static Color hsv2rgb(float h, float s, float v);

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
