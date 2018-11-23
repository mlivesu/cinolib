/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#ifndef CINO_COLOR_H
#define CINO_COLOR_H

#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>
#include <stdint.h>
#include <iostream>

namespace cinolib
{

class Color
{
    public:

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        static Color BLACK() { return Color( 0, 0, 0); }
        static Color WHITE() { return Color( 1, 1, 1); }
        //
        static Color RED    (const float lum = 1.0) { return Color( lum*1.0,     0.0,     0.0); }
        static Color GREEN  (const float lum = 1.0) { return Color(     0.0, lum*1.0,     0.0); }
        static Color BLUE   (const float lum = 1.0) { return Color(     0.0,     0.0, lum*1.0); }
        static Color YELLOW (const float lum = 1.0) { return Color( lum*1.0, lum*1.0,     0.0); }
        static Color MAGENTA(const float lum = 1.0) { return Color( lum*1.0,     0.0, lum*1.0); }
        static Color CYAN   (const float lum = 1.0) { return Color(     0.0, lum*1.0, lum*1.0); }
        static Color GRAY   (const float lum = 1.0) { return Color( lum*0.5, lum*0.5, lum*0.5); }
        //
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
              bool    operator<=(const Color & c) const;
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
        static Color parula_ramp(uint range, uint value);
        static Color quality2rgb(float q);
        static Color normal2rgb(const vec3d & n, bool flip_neg_z);
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
