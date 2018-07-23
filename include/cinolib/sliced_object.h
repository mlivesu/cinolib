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

#include <cinolib/meshes/trimesh.h>
#include <cinolib/boost_polygon_wrap.h>

/* This class represents a sliced object as a stack of polygons.
 * Silces are also triangulated for ease of processing, IO and rendering.
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

        explicit SlicedObj(const char * filename, const double thick_radius = 0.01);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit SlicedObj(const std::vector<std::vector<std::vector<vec3d>>> & slice_polys,
                           const std::vector<std::vector<std::vector<vec3d>>> & slice_holes,
                           const std::vector<std::vector<std::vector<vec3d>>> & supports,
                           const std::vector<std::vector<std::vector<vec3d>>> & hatches,
                           const double thick_radius = 0.01);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint num_slices() const { return slices.size(); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void  slice_segments(const uint sid, std::vector<vec3d> & verts, std::vector<uint> & segs) const;
        float slice_z       (const uint sid) const;
        bool  slice_contains(const uint sid, const vec2d & p) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    protected:

        void init(const std::vector<std::vector<std::vector<vec3d>>> & slice_polys,
                  const std::vector<std::vector<std::vector<vec3d>>> & slice_holes,
                  const std::vector<std::vector<std::vector<vec3d>>> & supports);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void triangulate_slices();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double                                       thick_radius; // supports thickening radius
        std::vector<float>                           z;            // per slice z-coord
        std::vector<BoostMultiPolygon>               slices;       // slices (included thickened supports)
        std::vector<std::vector<std::vector<vec3d>>> hatches;      // unused so far, just keeping them
};

}

#ifndef  CINO_STATIC_LIB
#include "sliced_object.cpp"
#endif

#endif // CINO_DRAWABLE_SLICED_OBJ
