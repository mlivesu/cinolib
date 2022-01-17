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
#include <cinolib/geometry/vec_mat.h>
#include <cinolib/color.h>
#include <cinolib/gl/draw_sphere.h>

namespace cinolib
{

class DrawableSphere : public DrawableObject
{
    public :

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

       ~DrawableSphere() override {}

        DrawableSphere(const vec3d & center = vec3d(0,0,0),
                       const float   radius = 1.0,
                       const Color & color  = Color::RED(),
                       const uint    subdiv = 1) // number of subdivisions of the regular icosahedron
            : center(center)
            , radius(radius)
            , color(color)
            , subdiv(subdiv)
        {
            // apply the subdivsion algorithm just once, filling the verts and tris vectors
            // Then you can render any time you want (scaling and translation are efficiently
            // handled at render time by opengl)
            icosphere(1.f, subdiv, verts, tris);
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d  center;
        double radius;
        Color  color;
        uint   subdiv;
        std::vector<float> verts;
        std::vector<uint>  tris;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        ObjectType object_type()  const override { return SPHERE_; }
        vec3d      scene_center() const override { return center;  }
        float      scene_radius() const override { return radius;  }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void draw(const float) const override
        {
            draw_sphere(center, radius, color, verts, tris);
        }

};

}

#endif // CINO_DRAWABLE_SPHERE_H
