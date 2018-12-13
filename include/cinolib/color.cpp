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
#include "color.h"
#include <cinolib/textures/texture_hsv.h>
#include <cinolib/textures/texture_parula.h>
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
bool Color::operator<=(const Color & c) const
{
    for(uint i=0; i<4; ++i)
    {
        if (this->operator[](i) <= c[i]) return true;
        if (this->operator[](i) >  c[i]) return false;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Color Color::red_white_blue_ramp_01(float val)
{
    if(val<0) return RED();
    if(val>1) return BLUE();

    if(val<=0.5)
    {
        val *= 2.0;
        return Color(WHITE().r * val + RED().r * (1.0 - val),
                     WHITE().g * val + RED().g * (1.0 - val),
                     WHITE().b * val + RED().b * (1.0 - val));
    }

    if(val<=1.0)
    {
        val = 2.0*val - 1.0;
        return Color(BLUE().r * val + WHITE().r * (1.0 - val),
                     BLUE().g * val + WHITE().g * (1.0 - val),
                     BLUE().b * val + WHITE().b * (1.0 - val));
    }

    assert(false);
    return Color(); // warning killer
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Color Color::red_ramp_01(float val)
{
    if(val<0) return RED();
    if(val>1) return WHITE();
    return Color(1,val,val);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Color Color::normal2rgb(const vec3d & n, bool flip_neg_z)
{
    double dir[3] =
    {
        n.x(),
        n.y(),
        (flip_neg_z) ? std::fabs(n.z()) : n.z()
    };

    float c[3];
    for(int i=0; i<3; ++i)
    {
        c[i] = (1.0 + dir[i])*0.5;
        if(c[i]<0) c[i]=0;
        if(c[i]>1) c[i]=1;
    }

    return Color(c[0], c[1], c[2]);
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

    float r = hsv_texture1D[3*i+0]/255.0;
    float g = hsv_texture1D[3*i+1]/255.0;
    float b = hsv_texture1D[3*i+2]/255.0;

    return Color(r,g,b);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Color Color::parula_ramp(uint n_colors, uint pos)
{
    assert(pos<n_colors);

    uint i = std::round(64 * static_cast<float>(pos)/static_cast<float>(n_colors));

    float r = parula_texture1D[3*i+0]/255.0;
    float g = parula_texture1D[3*i+1]/255.0;
    float b = parula_texture1D[3*i+2]/255.0;

    return Color(r,g,b);
}

}
