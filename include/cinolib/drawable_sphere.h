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
#ifndef CINO_DRAWABLE_SPHERE_H
#define CINO_DRAWABLE_SPHERE_H

#include <cinolib/drawable_object.h>
#include <cinolib/gl/draw_sphere.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/color.h>

namespace cinolib
{

class DrawableSphere : public DrawableObject
{
    public :

        DrawableSphere(const vec3d & center) : center(center)
        {
            size  = 1.0;
            color = Color::RED();
        }

        ~DrawableSphere(){}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d  center;
        Color  color;
        double size;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        ObjectType object_type()  const { return SPHERE; }
        vec3d      scene_center() const { return center; }
        float      scene_radius() const { return 0.0;    }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void draw(const float scene_size) const
        {
            sphere<vec3d>(center, size*scene_size/100.0, color.rgba);
        }
};

}

#endif // CINO_DRAWABLE_SPHERE_H
