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
float & Color::operator[](const uint i)
{
    return rgba[i];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//CINO_INLINE
//Color Color::HSV2RGB(float h, float s, float v)
//{
//    float tmp_r,tmp_g,tmp_b;
//    if(s==0.0) return Color(v,v,v);// gray color
//    if(h==1.0) h = 0.0;

//    int i   = int( floor(h*6.0) );
//    float f = float(h*6.0f - floor(h*6.0f));

//    float p = v*(1.0f-s);
//    float q = v*(1.0f-s*f);
//    float t = v*(1.0f-s*(1.0f-f));

//    switch(i)
//    {
//      case 0: tmp_r=v; tmp_g=t; tmp_b=p; break;
//      case 1: tmp_r=q; tmp_g=v; tmp_b=p; break;
//      case 2: tmp_r=p; tmp_g=v; tmp_b=t; break;
//      case 3: tmp_r=p; tmp_g=q; tmp_b=v; break;
//      case 4: tmp_r=t; tmp_g=p; tmp_b=v; break;
//      case 5: tmp_r=v; tmp_g=p; tmp_b=q; break;
//      default: tmp_r=0;tmp_g=0;tmp_b=0; assert(0);break;
//    }

//    return Color(tmp_r, tmp_g, tmp_b);
//}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

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

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Given an integer returns a well ordering of colors,  so that every color
// differs as much as possible form the previous one.
// Parameters:
//   n_colors => is the maximum expected value (max of the range)
//   pos      => is the requested position (it must be < n_colors);
//
CINO_INLINE
Color Color::scatter(uint range, uint value, float sat, float val)
{
    int b, k, m=range;
    int r =range;

          for (b=0, k=1; k<range; k<<=1)
                          if (value<<1>=m) {
                                  if (b==0) r = k;
                                  b += k;
                                  value -= (m+1)>>1;
                                  m >>= 1;
                          }
          else m = (m+1)>>1;
          if (r>range-b) r = range-b;

    return Color(0,0,0);// HSV2RGB(float(b)/float(range), sat, val);
}

//    float fpos  = float(value)/float(range - 1) * 255.0;
//    float alpha = fpos - std::floor(fpos);
//    uint  prev  = std::floor(fpos);
//    uint  next  = std::min(prev+1, (uint)255); // just to avoid "index out of bounds" errors...

//    int r = (1.0 - alpha) * quality_ramp_texture1D[prev*3 + 0] + alpha * quality_ramp_texture1D[next*3 + 0];
//    int g = (1.0 - alpha) * quality_ramp_texture1D[prev*3 + 1] + alpha * quality_ramp_texture1D[next*3 + 1];
//    int b = (1.0 - alpha) * quality_ramp_texture1D[prev*3 + 2] + alpha * quality_ramp_texture1D[next*3 + 2];

//    return Color(float(r)/255.0,
//                 float(g)/255.0,
//                 float(b)/255.0);

}
