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
#ifndef CINO_MESH_ATTRIBUTES_H
#define CINO_MESH_ATTRIBUTES_H

#include <cinolib/geometry/vec3.h>
#include <cinolib/color.h>
#include <string>

namespace cinolib
{

/* These are the MINIMAL attributes required by CinoLib.
 * If omitted, the library will not compile!!!!!!!!!!!
 *
 * NOTE: you do not need to specify them, they have been
 * set as default template arguments in any mesh available
 * within the library. Meshes can therefore be declared as
 * follows:
 *
 * Trimesh<>        my_trimesh;
 * Quadmesh<>       my_quadmesh;
 * Polygonmesh<>    my_polygonmesh;
 * Tetmesh<>        my_tetmesh;
 * Hexmesh<>        my_hexmesh;
 * Polyhedralmesh<> my_polyhedralmesh;
 *
 * Otherwise, if more attributes are necessary, you can always
 * extend the current structures, and use explicit templates:
 *
 * Trimesh<M,V,E,P>          my_trimesh;
 * Quadmesh<M,V,E,P>         my_quadmesh;
 * Polygonmesh<M,V,E,P>      my_polygonmesh;
 * Tetmesh<M,V,E,F,P>        my_tetmesh;
 * Hexmesh<M,V,E,F,P>        my_hexmesh;
 * Polyhedralmesh<M,V,E,F,P> my_hexmesh;
*/

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    std::string filename;
}
Mesh_std_attributes;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    vec3d  normal  = vec3d(0,0,0);
    Color  color   = Color::WHITE();
    vec3d  uvw     = vec3d(0,0,0);
    int    label   = -1;
    float  quality = 0.0;
    bool   marked  = false;
}
Vert_std_attributes;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    Color color  = Color::BLACK();
    bool  marked = false;
}
Edge_std_attributes;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    vec3d  normal  = vec3d(0,0,0);
    Color  color   = Color::WHITE();
    int    label   = -1;
    bool   visible = true;
    float  quality = 0.0;
    bool   marked  = false;
}
Polygon_std_attributes;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    Color  color   = Color::WHITE();
    int    label   = -1;
    bool   visible = true;
    float  quality = 0.0;
    bool   marked  = false;
}
Polyhedron_std_attributes;

}

#endif // CINO_MESH_ATTRIBUTES_H
