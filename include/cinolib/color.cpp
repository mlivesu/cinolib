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
#include <cinolib/textures/quality_ramp_texture.h>
#include <assert.h>
#include <algorithm>
#include <cmath>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Color & c)
{
    in << "r:" << c.r << " g:" << c.g << " b:" << c.b << " a:" << c.a << " ";
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
const float & Color::operator[](const uint i) const
{
    return rgba[i];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Color & Color::operator*=(const double d)
{
    r*=d;
    g*=d;
    b*=d;
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
float & Color::operator[](const uint i)
{
    return rgba[i];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Color::operator==(const Color & c) const
{
    for(uint i=0; i<4; ++i)
    {
        if (this->operator[](i) != c[i]) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Color::operator!=(const Color & c) const
{
    for(uint i=0; i<4; ++i)
    {
        if (this->operator[](i) != c[i]) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Color::operator<(const Color & c) const
{
    for(uint i=0; i<4; ++i)
    {
        if (this->operator[](i) < c[i]) return true;
        if (this->operator[](i) > c[i]) return false;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Color Color::quality2rgb(float q)
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
    return Color(); // warning killer
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Color Color::hsv2rgb(float h, float s, float v)
{
    if(s==0.0) return Color(v,v,v); // gray color
    if(h==1.0) h = 0.0;

    int i   = int( floor(h*6.0) );
    float f = float(h*6.0f - floor(h*6.0f));

    float p = v*(1.0f-s);
    float q = v*(1.0f-s*f);
    float t = v*(1.0f-s*(1.0f-f));

    switch(i)
    {
        case 0: return Color(v,t,p);
        case 1: return Color(q,v,p);
        case 2: return Color(p,v,t);
        case 3: return Color(p,q,v);
        case 4: return Color(t,p,v);
        case 5: return Color(v,p,q);
        default: assert(false);
    }
    return Color(); // warning killer
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Color Color::scatter(uint n_colors, uint pos, float sat, float val)
{
    n_colors += 1; // for some reason I am getting duplicated colors without this... :(

    assert(pos<n_colors);

    // Magic stolen from VCG :P

    uint b, k, m = n_colors;
    uint r = n_colors;

    for (b=0, k=1; k<n_colors; k<<=1)
    {
        if (pos<<1>=m)
        {
            if (b==0) r = k;
            b += k;
            pos -= (m+1)>>1;
            m >>= 1;
        }
        else m = (m+1)>>1;
        if (r>n_colors-b) r = n_colors-b;
    }

    return hsv2rgb(float(b)/float(n_colors), sat, val);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Color Color::hsv_ramp(uint n_colors, uint pos)
{
    assert(pos<n_colors);

    uint i = std::round(255 * static_cast<float>(pos)/static_cast<float>(n_colors));

    float r = quality_ramp_texture1D[3*i+0]/255.0;
    float g = quality_ramp_texture1D[3*i+1]/255.0;
    float b = quality_ramp_texture1D[3*i+2]/255.0;

    return Color(r,g,b);
}

}
