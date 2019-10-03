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
#ifndef CINO_DRAWABLE_OBJECT_H
#define CINO_DRAWABLE_OBJECT_H

#include <cinolib/geometry/vec3.h>

namespace cinolib
{

typedef enum
{
    DRAWABLE_TRIMESH       ,
    DRAWABLE_TETMESH       ,
    DRAWABLE_QUADMESH      ,
    DRAWABLE_HEXMESH       ,
    DRAWABLE_POLYGONMESH   ,
    DRAWABLE_POLYHEDRALMESH,
    DRAWABLE_SKELETON      ,
    DRAWABLE_CURVE         ,
    DRAWABLE_ISOSURFACE    ,
    DRAWABLE_SLICED_OBJ    ,
    DRAWABLE_VECTOR_FIELD  ,
    DRAWABLE_OCTREE        ,
    ARROW                  ,
    SPHERE
}
ObjectType;

class DrawableObject
{
    public :

        explicit  DrawableObject(){}
        virtual  ~DrawableObject(){}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual  ObjectType  object_type()                    const = 0;
        virtual  void        draw(const float scene_size = 1) const = 0;  // do rendering
        virtual  vec3d       scene_center()                   const = 0;  // get position in space
        virtual  float       scene_radius()                   const = 0;  // get size (approx. radius of the bounding sphere)
};

}

#endif // CINO_DRAWABLE_OBJECT_H
