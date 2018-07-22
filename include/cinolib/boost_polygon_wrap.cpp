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
#include <cinolib/boost_polygon_wrap.h>
#include <cinolib/vector_serialization.h>

#ifdef CINOLIB_USES_TRIANGLE
#include <cinolib/triangle_wrap.h>
#endif

namespace cinolib
{

template<typename vec>
CINO_INLINE
BoostPolygon make_polygon(const std::vector<vec> & outer_ring)
{
    assert(outer_ring.size()>2); // make sure it is a closed polygon
    BoostPolygon poly;
    for(vec p : outer_ring) boost::geometry::append(poly, BoostPoint(p.x(), p.y()));
    boost::geometry::correct(poly);
    return poly;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename vec>
CINO_INLINE
BoostPolygon make_polygon(const std::vector<vec>              & outer_ring,
                          const std::vector<std::vector<vec>> & inner_rings)
{
    assert(outer_ring.size()>1);
    assert(!inner_rings.empty());
    // TODO!!
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename vec>
CINO_INLINE
BoostPolygon make_polygon(const std::vector<vec> & polyline,
                          const double             thickening_radius)
{
    assert(polyline.size()>1); // make sure it is at least a segment
    assert(thickening_radius > 0);

    BoostLinestring ls;
    for(vec p : polyline) boost::geometry::append(ls, BoostPoint(p.x(), p.y()));

    // https://www.boost.org/doc/libs/1_63_0/libs/geometry/doc/html/geometry/reference/algorithms/buffer/buffer_7_with_strategies.html
    boost::geometry::strategy::buffer::distance_symmetric<double> distance_strategy(thickening_radius);
    boost::geometry::strategy::buffer::join_miter                 join_strategy;
    boost::geometry::strategy::buffer::end_flat                   end_strategy;
    boost::geometry::strategy::buffer::point_square               circle_strategy;
    boost::geometry::strategy::buffer::side_straight              side_strategy;
    std::vector<BoostPolygon> res;
    boost::geometry::buffer(ls, res, distance_strategy, side_strategy, join_strategy, end_strategy, circle_strategy);

    // topological check: thickening should not create more than one polygon
    assert(res.size()==1);

    // buffering tends to create degenerate edges. Clean the result with a pass of Douglas-Peucker
    return polygon_simplify(res.front(), thickening_radius*0.01);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename TP>
CINO_INLINE
TP polygon_simplify(const TP & p, const double max_dist)
{
    TP simplified_p;
    boost::geometry::simplify(p, simplified_p, max_dist); // Douglas-Peucker
    return simplified_p;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename TP, typename vec>
CINO_INLINE
bool polygon_contains(const TP & poly, const vec & point, const bool border_counts)
{
    double x = point.x();
    double y = point.y();

    if(border_counts) return boost::geometry::covered_by(BoostPoint(x,y), poly);
                      return boost::geometry::within    (BoostPoint(x,y), poly);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename TP0, typename TP1>
CINO_INLINE
BoostMultiPolygon polygon_union(const TP0 & p0, const TP1 & p1)
{
    BoostMultiPolygon res;
    boost::geometry::union_(p0, p1, res);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename TP0, typename TP1>
CINO_INLINE
BoostMultiPolygon polygon_difference(const TP0 & p0, const TP1 & p1)
{
    BoostMultiPolygon res;
    boost::geometry::difference(p0, p1, res);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void polygon_get_edges(const BoostPolygon       & poly,
                             std::vector<vec2d> & verts,
                             std::vector<uint>  & edges)
{
    verts.clear();
    edges.clear();

    // extract outer edges
    uint nv = poly.outer().size()-1; // first and last verts coincide...
    for(uint vid=0; vid<nv; ++vid)
    {
        verts.push_back(vec2d(boost::geometry::get<0>(poly.outer().at(vid)),
                              boost::geometry::get<1>(poly.outer().at(vid))));
        edges.push_back(vid);
        edges.push_back((vid+1)%nv);
    }

    // extract inner edges (holes)
    for(uint hid=0; hid<poly.inners().size(); ++hid)
    {
        uint base_addr = verts.size();
        uint nv = poly.inners().at(hid).size()-1; // first and last verts coincide...
        for(uint vid=0; vid<nv; ++vid)
        {
            verts.push_back(vec2d(boost::geometry::get<0>(poly.inners().at(hid).at(vid)),
                                  boost::geometry::get<1>(poly.inners().at(hid).at(vid))));
            edges.push_back(base_addr + vid);
            edges.push_back(base_addr + (vid+1)%nv);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void polygon_get_edges(const BoostMultiPolygon  & poly,
                             std::vector<vec2d> & verts,
                             std::vector<uint>  & edges)
{
    verts.clear();
    edges.clear();

    for(const BoostPolygon & p : poly)
    {
        std::vector<vec2d> v;
        std::vector<uint>  e;
        polygon_get_edges(p, v, e);

        uint base_addr = verts.size();
        std::copy(v.begin(), v.end(), std::back_inserter(verts));
        for(auto vid : e) edges.push_back(base_addr + vid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void polygon_get_edges(const BoostPolygon       & poly,
                       const double             & z, // add third coordinate
                             std::vector<vec3d> & verts,
                             std::vector<uint>  & edges)
{
    std::vector<vec2d> v2d;
    polygon_get_edges(poly, v2d, edges);
    verts = vec3d_from_vec2d(v2d, z);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void polygon_get_edges(const BoostMultiPolygon  & poly,
                       const double             & z, // add third coordinate
                             std::vector<vec3d> & verts,
                             std::vector<uint>  & edges)
{
    std::vector<vec2d> v2d;
    polygon_get_edges(poly, v2d, edges);
    verts = vec3d_from_vec2d(v2d, z);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<typename TP>
//CINO_INLINE
//void triangulate_polygon(const TP                 & poly,
//                               std::vector<vec3d> & verts,
//                               std::vector<uint>  & tris)
//{
//    verts.clear();
//    tris.clear();

//    std::vector<vec3d> v;
//    std::vector<uint>  e;
//    polygon_get_edges(poly, 0, v, e);

//    triangle_wrap(v, e, {}, 0, "", verts, tris);
//}

}
