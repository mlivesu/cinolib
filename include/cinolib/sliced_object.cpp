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
#include <cinolib/sliced_object.h>
#include <cinolib/io/read_CLI.h>
#include <cinolib/triangle_wrap.h>
#include <cinolib/vector_serialization.h>
#include <cinolib/ANSI_color_codes.h>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/linestring.hpp>

typedef boost::geometry::model::d2::point_xy<double>        BoostPoint;
typedef boost::geometry::model::polygon<BoostPoint>         BoostPolygon;
typedef boost::geometry::model::multi_polygon<BoostPolygon> BoostMultiPolygon;
typedef boost::geometry::model::linestring<BoostPoint>      BoostLinestring;

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
SlicedObj<M,V,E,P>::SlicedObj(const char * filename, const double hatch_size)
    : Trimesh<M,V,E,P>()
    , hatch_size(hatch_size)
{
    std::vector<std::vector<std::vector<vec3d>>> internal_polylines;
    std::vector<std::vector<std::vector<vec3d>>> external_polylines;
    std::vector<std::vector<std::vector<vec3d>>> open_polylines;
    std::vector<std::vector<std::vector<vec3d>>> hatches;
    read_CLI(filename,internal_polylines,external_polylines,open_polylines,hatches);

    init(internal_polylines, external_polylines, open_polylines, hatches);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
SlicedObj<M,V,E,P>::SlicedObj(const std::vector<std::vector<std::vector<vec3d>>> & internal_polylines,
                              const std::vector<std::vector<std::vector<vec3d>>> & external_polylines,
                              const std::vector<std::vector<std::vector<vec3d>>> & open_polylines,
                              const std::vector<std::vector<std::vector<vec3d>>> & hatches,
                              const double hatch_size)
    : Trimesh<M,V,E,P>()
    , hatch_size(hatch_size)
{
    init(internal_polylines, external_polylines, open_polylines, hatches);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void SlicedObj<M,V,E,P>::init(const std::vector<std::vector<std::vector<vec3d>>> & internal_polylines,
                              const std::vector<std::vector<std::vector<vec3d>>> & external_polylines,
                              const std::vector<std::vector<std::vector<vec3d>>> & open_polylines,
                              const std::vector<std::vector<std::vector<vec3d>>> & hatches)
{
    std::cout << "new sliced object (" << internal_polylines.size() << " slices)" << std::endl;

    (void)hatches; // warning killer => DO SOMETHING WITH THEM!

    triangulate_slices(internal_polylines, external_polylines, open_polylines);
    this->edge_mark_boundaries();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void SlicedObj<M,V,E,P>::triangulate_slices(const std::vector<std::vector<std::vector<vec3d>>> & internal_polylines,
                                            const std::vector<std::vector<std::vector<vec3d>>> & external_polylines,
                                            const std::vector<std::vector<std::vector<vec3d>>> & open_polylines)
{
    uint n_slices = external_polylines.size();

    for(uint sid=0; sid<n_slices; ++sid)
    {        
        bool has_external_polyline = !external_polylines.at(sid).empty();
        bool has_open_polyline     = !open_polylines.at(sid).empty();

        double z;
        if(has_external_polyline) z = external_polylines.at(sid).front().front().z(); else
        if(has_open_polyline    ) z = open_polylines.at(sid).front().front().z();     else
        {
            std::cout << ANSI_fg_color_red << "WARNING: slice " << sid << " is empty" << ANSI_fg_color_default << std::endl;
            continue;
        }

        std::vector<double> verts_in, verts_out;
        std::vector<uint>   segs_in, tris_out;
        uint base_addr = 0;

        // process external polylines
        for(const std::vector<vec3d> & polyline : external_polylines.at(sid))
        {
            for(uint i=0; i<polyline.size(); ++i)
            {
                verts_in.push_back(polyline.at(i).x());
                verts_in.push_back(polyline.at(i).y());
                segs_in.push_back(base_addr + i);
                segs_in.push_back(base_addr + (i+1)%polyline.size());
            }
            base_addr = verts_in.size()/2;
        }
        // process internal polylines (i.e. holes)
        for(const std::vector<vec3d> & polyline : internal_polylines.at(sid))
        {
            for(uint i=0; i<polyline.size(); ++i)
            {
                verts_in.push_back(polyline.at(i).x());
                verts_in.push_back(polyline.at(i).y());
                segs_in.push_back(base_addr + i);
                segs_in.push_back(base_addr + (i+1)%polyline.size());
            }
            base_addr = verts_in.size()/2;
        }

        // find robust seeds to eat triangles inside holes
        std::vector<double> holes_in;
        points_inside_holes(internal_polylines.at(sid), holes_in);

        // WARNING: thickening may create intersections between open polylines and the
        // rest of the slice. If this is the case, should I do anything about it?
        thicken_open_polylines(open_polylines.at(sid), hatch_size, verts_in, segs_in);

        if(!verts_in.empty())
        {
            triangle_wrap(verts_in, segs_in, holes_in, "Q", verts_out, tris_out);

            base_addr   = this->num_verts();
            uint n_tris = tris_out.size()/3;

            std::vector<vec3d> verts = vec3d_from_serialized_xy(verts_out, z);

            for(auto p : verts)
            {
                uint vid = this->vert_add(p);
                this->vert_data(vid).uvw[0] = static_cast<double>(sid)/static_cast<double>(n_slices);
            }
            for(uint i=0; i<n_tris; ++i)
            {
                uint pid = this->poly_add(base_addr + tris_out.at(3*i+0),
                                          base_addr + tris_out.at(3*i+1),
                                          base_addr + tris_out.at(3*i+2));
                this->poly_data(pid).label = sid;
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/* This serves to reliably find an inner point for each internal contour (a slice hole).
 * Triangle uses this information to eat triangles inside holes and clean the triangulation.
 * Basically each contour is triangulated, and the centroid of the first triangle in the
 * tessellation is returned.
*/
template<class M, class V, class E, class P>
CINO_INLINE
void SlicedObj<M,V,E,P>::points_inside_holes(const std::vector<std::vector<vec3d>> & holes,
                                                   std::vector<double>             & points)
{
    points.clear();
    for(const std::vector<vec3d> & hole : holes)
    {
        std::vector<double> verts_in, verts_out, dummy;
        std::vector<uint>   segs_in, tris_out;
        for(uint i=0; i<hole.size(); ++i)
        {
            verts_in.push_back(hole.at(i).x());
            verts_in.push_back(hole.at(i).y());
            segs_in.push_back(i);
            segs_in.push_back((i+1)%hole.size());
        }

        triangle_wrap(verts_in, segs_in, dummy, "Q", verts_out, tris_out);
        assert(tris_out.size()>2);
        uint v0 = tris_out.at(0);
        uint v1 = tris_out.at(1);
        uint v2 = tris_out.at(2);
        points.push_back((verts_out.at(2*v0+0) + verts_out.at(2*v1+0) + verts_out.at(2*v2+0))/3.0);
        points.push_back((verts_out.at(2*v0+1) + verts_out.at(2*v1+1) + verts_out.at(2*v2+1))/3.0);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void SlicedObj<M,V,E,P>::thicken_open_polylines(const std::vector<std::vector<vec3d>> & open_polylines,
                                                const double                          & thickness,
                                                      std::vector<double>             & points,
                                                      std::vector<uint>               & segs)
{
    if(thickness<=0) return;

    // https://www.boost.org/doc/libs/1_63_0/libs/geometry/doc/html/geometry/reference/algorithms/buffer/buffer_7_with_strategies.html
    boost::geometry::strategy::buffer::distance_symmetric<double> distance_strategy(thickness);
    boost::geometry::strategy::buffer::join_miter                 join_strategy;
    boost::geometry::strategy::buffer::end_flat                   end_strategy;
    boost::geometry::strategy::buffer::point_square               circle_strategy;
    boost::geometry::strategy::buffer::side_straight              side_strategy;

    std::vector<BoostPolygon> buffered_polylines;
    for(auto polyline : open_polylines)
    {
        BoostLinestring ls;
        for(uint i=0; i<polyline.size(); ++i)
        {
            boost::geometry::append(ls, BoostPoint(polyline.at(i).x(),polyline.at(i).y()));
        }

        std::vector<BoostPolygon> res;
        boost::geometry::buffer(ls, res, distance_strategy, side_strategy, join_strategy, end_strategy, circle_strategy);
        assert(res.size()==1); // topological check: thickening should not create more than one conn. comp.
        for(auto p : res) buffered_polylines.push_back(p);
    }

    // open polylines may intersect. So I unify them all in a single (multi)polygon
    // and simplify them to remove spurious vertices
    BoostMultiPolygon unified_polylines;
    for(uint i=0; i<buffered_polylines.size(); ++i)
    {
        BoostMultiPolygon res;
        boost::geometry::union_(unified_polylines, buffered_polylines.at(i), res);
        unified_polylines = res;
    }
    // Douglas Peucker
    BoostMultiPolygon simplified_polylines;
    boost::geometry::simplify(unified_polylines, simplified_polylines, thickness*0.01);

    for(uint i=0; i<simplified_polylines.size(); ++i)
    {
        uint base_addr = points.size()/2;
        for(uint j=0; j<simplified_polylines[i].outer().size()-1; ++j)
        {
            points.push_back(boost::geometry::get<0>(simplified_polylines[i].outer()[j]));
            points.push_back(boost::geometry::get<1>(simplified_polylines[i].outer()[j]));
            segs.push_back(base_addr + j);
            segs.push_back(base_addr + (j+1)%(simplified_polylines[i].outer().size()-1));
        }

        base_addr = points.size()/2;
        for(auto p : simplified_polylines[i].inners())
        for(uint j=0; j<p.size()-1; ++j)
        {
            points.push_back(boost::geometry::get<0>(p[j]));
            points.push_back(boost::geometry::get<1>(p[j]));
            segs.push_back(base_addr + j);
            segs.push_back(base_addr + (j+1)%(p.size()-1));
        }
    }
}

}
