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

        BoostMultiPolygon slice_as_boost_poly(const uint sid) const;
        void              slice_segments     (const uint sid, std::vector<vec3d> & verts, std::vector<uint> & segs) const;
        float             slice_z            (const uint sid) const;
        float             slice_thickness    (const uint sid) const;
        bool              slice_contains     (const uint sid, const vec2d & p) const;
        float             slice_avg_thickness() const;

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
