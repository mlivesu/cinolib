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
#ifndef CINO_DRAWABLE_OBJECT_H
#define CINO_DRAWABLE_OBJECT_H

#include <cinolib/geometry/vec3.h>
#include <float.h>

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
    DRAWABLE_VECTOR_FIELD
}
ObjectType;

class DrawableObject
{
    public :

        virtual ObjectType  object_type()                    const = 0;
        virtual void        draw(const float scene_size = 1) const = 0;  // do rendering
        virtual vec3d       scene_center()                   const = 0;  // get position in space
        virtual float       scene_radius()                   const = 0;  // get size (approx. radius of the bounding sphere)
        virtual void        slice(const float,   // thresh,
                                  const int  ,   // item = { X, Y, Z, L, Q }
                                  const int  ,   // sign = { LEQ, GEQ }
                                  const int  ){} // mode = { AND , OR }
};

}

#endif // CINO_DRAWABLE_OBJECT_H
