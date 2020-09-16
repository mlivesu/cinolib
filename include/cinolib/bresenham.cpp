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
#include <cinolib/bresenham.h>

namespace cinolib
{

CINO_INLINE
std::vector<std::pair<short,short>> bresenham_line(short x0, short y0, short x1, short y1)
{
    std::vector<std::pair<short,short>> line;

    bool steep = std::abs(y1 - y0) > std::abs(x1 - x0),reverse = (x0 > x1);
    if (steep) { std::swap(x0,y0); std::swap(x1,y1); }
    if (reverse) { std::swap(x0,x1); std::swap(y0,y1); }

    short delta_x = x1 - x0,delta_y=std::abs(y1 - y0),error=delta_x/2,y_step=(y0 < y1)?1:-1,y=y0;
    
    for(short x=x0; x<=x1; ++x)
    {
        line.push_back(steep ? std::make_pair(y,x) : std::make_pair(x,y));

        if ((error -= delta_y) < 0)
        {
            y += y_step;
            error += delta_x;
        }
    }

    if (reverse) std::reverse(line.begin(), line.end());
    return line;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<std::pair<short,short>> bresenham_circle(short cx, short cy, short radius)
{
    std::vector<std::pair<short,short>> circle;

    short x=0,y=radius,g=3-2*radius,diag_inc=10-4*radius,right_inc=6;
    

    while(x <= y)
    {
        circle.push_back(std::make_pair( x+cx,  y+cy));
        circle.push_back(std::make_pair( x+cx, -y+cy));
        circle.push_back(std::make_pair(-x+cx,  y+cy));
        circle.push_back(std::make_pair(-x+cx, -y+cy));
        circle.push_back(std::make_pair( y+cx,  x+cy));
        circle.push_back(std::make_pair( y+cx, -x+cy));
        circle.push_back(std::make_pair(-y+cx,  x+cy));
        circle.push_back(std::make_pair(-y+cx, -x+cy));

        if (g>=0)
        {
            g += diag_inc;
            diag_inc += 8;
            y -= 1;
        }
        else
        {
            g += right_inc;
            diag_inc += 4;
        }

        right_inc += 4;
        x += 1;
    }
    return circle;
}

}
