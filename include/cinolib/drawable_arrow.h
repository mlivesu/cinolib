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
#ifndef CINO_DRAWABLE_ARROW_H
#define CINO_DRAWABLE_ARROW_H

#include <cinolib/drawable_object.h>
#include <cinolib/gl/draw_arrow.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/color.h>

namespace cinolib
{

class DrawableArrow : public DrawableObject
{
    public :

        DrawableArrow(const vec3d & beg, const vec3d & end) : beg(beg), end(end)
        {
            size  = beg.dist(end) * 0.1;
            color = Color::RED();
        }

        ~DrawableArrow(){}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d  beg, end;
        Color  color;
        double size;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        ObjectType object_type()  const { return ARROW; }
        vec3d      scene_center() const { return (beg+end)*0.5; }
        float      scene_radius() const { return beg.dist(end); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void draw(const float) const
        {
            arrow<vec3d>(beg, end, size, color.rgba);
        }
};

}

#endif // CINO_DRAWABLE_ARROW_H
