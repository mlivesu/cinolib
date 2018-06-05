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
#ifndef CINO_SLICED_OBJ
#define CINO_SLICED_OBJ

// Boost polygons and Triangle are used to create and tessellate slices...
#ifdef CINOLIB_USES_BOOST
#ifdef CINOLIB_USES_TRIANGLE

#include <cinolib/meshes/trimesh.h>

/* This class represents a sliced object as a triangle mesh. Each slice is triangulated,
 * considering both outer and inner contours (holes). Support structures (usually represented
 * as 1D curves in CLI files) are thickened and triangulated as well.
*/

namespace cinolib
{

template<class M = Mesh_std_attributes, // default template arguments
         class V = Vert_std_attributes,
         class E = Edge_std_attributes,
         class P = Polygon_std_attributes>
class SlicedObj : public Trimesh<M,V,E,P>
{
    public:

        explicit SlicedObj(){}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit SlicedObj(const char * filename, const double hatch_size = 0.01);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit SlicedObj(const std::vector<std::vector<std::vector<vec3d>>> & internal_polylines,
                           const std::vector<std::vector<std::vector<vec3d>>> & external_polylines,
                           const std::vector<std::vector<std::vector<vec3d>>> & open_polylines,
                           const std::vector<std::vector<std::vector<vec3d>>> & hatches,
                           const double hatch_size = 0.01);


        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    protected:

        void init(const std::vector<std::vector<std::vector<vec3d>>> & internal_polylines,
                  const std::vector<std::vector<std::vector<vec3d>>> & external_polylines,
                  const std::vector<std::vector<std::vector<vec3d>>> & open_polylines,
                  const std::vector<std::vector<std::vector<vec3d>>> & hatches);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void triangulate_slices(const std::vector<std::vector<std::vector<vec3d>>> & internal_polylines,
                                const std::vector<std::vector<std::vector<vec3d>>> & external_polylines,
                                const std::vector<std::vector<std::vector<vec3d>>> & open_polylines);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void points_inside_holes(const std::vector<std::vector<vec3d>> & internal_polylines,
                                       std::vector<double>             & holes);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void thicken_open_polylines(const std::vector<std::vector<vec3d>> & open_polylines,
                                    const double                          & thickness,
                                          std::vector<double>             & verts,
                                          std::vector<uint>               & segs);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double hatch_size;
};

}

#ifndef  CINO_STATIC_LIB
#include "sliced_object.cpp"
#endif

#endif // CINO_USES_TRIANGLE
#endif // CINO_USES_BOOST

#endif // CINO_DRAWABLE_SLICED_OBJ
