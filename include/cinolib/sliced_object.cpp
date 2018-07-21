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
    // Empty slices will be ignored, so keep a separate count of slice ids.
    // NOTE: empty slices may happen when a slice contains only open polylines
    // and the hatch_size is set to zero
    uint sid        = 0;
    uint tot_slices = internal_polylines.size();

    for(uint i=0; i<tot_slices; ++i)
    {
        uint np = external_polylines.at(i).size();
        uint nh = internal_polylines.at(i).size();
        uint ns = (hatch_size>0) ? open_polylines.at(i).size() : 0;

        std::cout << "processing slice " << i << " out of " << tot_slices << "\t(" << np << " polys / " << nh << " holes / "  << ns << " supports)" << std::endl;

        if(np>0) z.push_back(external_polylines.at(i).front().front().z()); else
        if(ns>0) z.push_back(open_polylines.at(i).front().front().z());     else
        continue; // empty slice, skip it

        std::vector<BoostPolygon> polys;
        std::vector<BoostPolygon> holes;
        for(auto p : external_polylines.at(i)) polys.push_back(make_polygon(p));
        for(auto p : internal_polylines.at(i)) holes.push_back(make_polygon(p));
        if(hatch_size>0)
        {
            for(auto p : open_polylines.at(i)) polys.push_back(thicken_hatch(p, hatch_size));
        }

        BoostMultiPolygon mp;
        for(auto p : polys)
        {
            BoostMultiPolygon res;
            boost::geometry::union_(mp, p, res);
            mp = res;
        }
        for(auto p : holes)
        {
            BoostMultiPolygon res;
            boost::geometry::difference(mp, p, res);
            mp = res;
        }

        BoostMultiPolygon mp2;
        boost::geometry::simplify(mp, mp2, hatch_size*0.01); // Douglas-Peucker
        assert(mp.size()>0 && mp.size()==mp2.size());

        std::vector<double> verts_in, verts_out, hole_seeds;
        std::vector<uint> segs, tris;
        points_inside_holes(mp2, hole_seeds);
        for(const BoostPolygon & p : mp)
        {
            uint base_addr = verts_in.size()/2;
            uint nv = p.outer().size()-1; // first and last verts coincide...
            for(uint vid=0; vid<nv; ++vid)
            {
                verts_in.push_back(boost::geometry::get<0>(p.outer().at(vid)));
                verts_in.push_back(boost::geometry::get<1>(p.outer().at(vid)));
                segs.push_back(base_addr + vid);
                segs.push_back(base_addr + (vid+1)%nv);
            }
            for(uint hid=0; hid<p.inners().size(); ++hid)
            {
                uint base_addr = verts_in.size()/2;
                uint nv = p.inners().at(hid).size()-1; // first and last verts coincide...
                for(uint vid=0; vid<nv; ++vid)
                {
                    verts_in.push_back(boost::geometry::get<0>(p.inners().at(hid).at(vid)));
                    verts_in.push_back(boost::geometry::get<1>(p.inners().at(hid).at(vid)));
                    segs.push_back(base_addr + vid);
                    segs.push_back(base_addr + (vid+1)%nv);
                }
            }
        }
        triangle_wrap(verts_in, segs, hole_seeds, "Q", verts_out, tris);

        uint base_addr = this->num_verts();
        uint n_tris    = tris.size()/3;
        std::vector<vec3d> verts = vec3d_from_serialized_xy(verts_out, z.back());

        for(auto p : verts)
        {
            uint vid = this->vert_add(p);
            this->vert_data(vid).uvw[0] = static_cast<double>(i)/static_cast<double>(tot_slices);
            this->vert_data(vid).label  = sid;
        }
        for(uint i=0; i<n_tris; ++i)
        {
            uint pid = this->poly_add(base_addr + tris.at(3*i+0),
                                      base_addr + tris.at(3*i+1),
                                      base_addr + tris.at(3*i+2));
            this->poly_data(pid).label = sid;
            for(uint eid : this->adj_p2e(pid)) this->edge_data(eid).label = sid;
        }

        ++sid;
    }
    std::cout << std::endl;
    std::cout << "new sliced object (" << num_slices() << " slices)" << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/* This serves to reliably find an inner point for each hole in the slice. Triangle
 * uses this information to eat triangles inside holes and clean the triangulation.
 * Basically each contour is triangulated, and the centroid of the first triangle in
 * the tessellation is returned.
*/
template<class M, class V, class E, class P>
CINO_INLINE
void SlicedObj<M,V,E,P>::points_inside_holes(const BoostMultiPolygon   & mp,
                                                   std::vector<double> & points) const
{
    points.clear();
    for(const BoostPolygon & p : mp)
    {
        for(uint hid=0; hid<p.inners().size(); ++hid)
        {
            std::vector<double> verts_in, verts_out, dummy;
            std::vector<uint>   segs_in, tris_out;
            uint nv = p.inners().at(hid).size()-1; // first and last verts coincide...
            for(uint i=0; i<nv; ++i)
            {
                verts_in.push_back(boost::geometry::get<0>(p.inners().at(hid).at(i)));
                verts_in.push_back(boost::geometry::get<1>(p.inners().at(hid).at(i)));
                segs_in.push_back(i);
                segs_in.push_back((i+1)%nv);
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
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
BoostPolygon SlicedObj<M,V,E,P>::thicken_hatch(const std::vector<vec3d> & polyline,
                                               const double               thickness) const
{
    assert(polyline.size()>1); // make sure it is at least a segment
    assert(thickness > 0);

    BoostLinestring ls;
    for(vec3d p : polyline) boost::geometry::append(ls, BoostPoint(p.x(), p.y()));

    // https://www.boost.org/doc/libs/1_63_0/libs/geometry/doc/html/geometry/reference/algorithms/buffer/buffer_7_with_strategies.html
    boost::geometry::strategy::buffer::distance_symmetric<double> distance_strategy(thickness);
    boost::geometry::strategy::buffer::join_miter                 join_strategy;
    boost::geometry::strategy::buffer::end_flat                   end_strategy;
    boost::geometry::strategy::buffer::point_square               circle_strategy;
    boost::geometry::strategy::buffer::side_straight              side_strategy;
    std::vector<BoostPolygon> res;
    boost::geometry::buffer(ls, res, distance_strategy, side_strategy, join_strategy, end_strategy, circle_strategy);
    assert(res.size()==1); // topological check: thickening should not create more than one polygon

    BoostPolygon p;
    boost::geometry::simplify(res.front(), p, thickness*0.01); // Douglas-Peucker

    return p;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
BoostPolygon SlicedObj<M,V,E,P>::make_polygon(const std::vector<vec3d> & polyline) const
{
    assert(polyline.size()>2); // make sure it is a closed polygon
    BoostPolygon poly;
    for(vec3d p : polyline) boost::geometry::append(poly, BoostPoint(p.x(), p.y()));
    boost::geometry::correct(poly);
    return poly;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void SlicedObj<M,V,E,P>::slice_segments(const uint           sid,
                                        std::vector<vec3d> & verts,
                                        std::vector<uint>  & segs) const
{
    verts.clear();
    segs.clear();
    std::map<uint,uint> v_map;
    uint fresh_id = 0;
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if(this->edge_is_boundary(eid) && this->edge_data(eid).label == (int)sid)
        {
            for(uint off=0; off<2; ++off)
            {
                uint vid   = this->edge_vert_id(eid,off);
                auto query = v_map.find(vid);
                if(query == v_map.end())
                {
                    verts.push_back(this->vert(vid));
                    v_map[vid] = fresh_id;
                    vid = fresh_id;
                    fresh_id++;
                } else vid = query->second;
                segs.push_back(vid);
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
float SlicedObj<M,V,E,P>::slice_z(const uint sid) const
{
    return z.at(sid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool SlicedObj<M,V,E,P>::slice_contains(const uint sid, const vec2d & p) const
{
    for(uint pid=0; pid<this->num_polys(); ++pid)
    {
        if(this->poly_data(pid).label == (int)sid)
        {
            if(triangle_point_is_inside(vec2d(this->poly_vert(pid,0)),
                                        vec2d(this->poly_vert(pid,1)),
                                        vec2d(this->poly_vert(pid,2)),
                                        p))
                return true;
        }
    }
    return false;
}

}
