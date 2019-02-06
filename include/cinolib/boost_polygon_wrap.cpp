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
#include <cinolib/boost_polygon_wrap.h>
#include <cinolib/vector_serialization.h>

#ifdef CINOLIB_USES_TRIANGLE
#include <cinolib/triangle_wrap.h>
#endif

namespace cinolib
{

template<typename Point>
CINO_INLINE
BoostPolygon make_polygon(const std::vector<Point> & outer_ring)
{
    assert(outer_ring.size()>2); // make sure it is a closed polygon
    BoostPolygon poly;
    for(Point p : outer_ring) boost::geometry::append(poly, BoostPoint(p.x(), p.y()));
    boost::geometry::correct(poly);
    return poly;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename Point>
CINO_INLINE
BoostPolygon make_polygon(const std::vector<Point>              & outer_ring,
                          const std::vector<std::vector<Point>> & inner_rings)
{
    assert(outer_ring.size()>1);
    assert(!inner_rings.empty());
    // TODO!!
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename Point>
CINO_INLINE
BoostPolygon make_polygon(const std::vector<Point> & polyline,
                          const double             thickening_radius)
{
    assert(polyline.size()>1); // make sure it is at least a segment
    assert(thickening_radius > 0);

    BoostLinestring ls;
    for(Point p : polyline) boost::geometry::append(ls, BoostPoint(p.x(), p.y()));

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

template<typename Poly>
CINO_INLINE
Poly polygon_simplify(const Poly & p, const double max_dist)
{
    Poly simplified_p;
    boost::geometry::simplify(p, simplified_p, max_dist); // Douglas-Peucker
    return simplified_p;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename Poly>
CINO_INLINE
double polygon_area(const Poly & p)
{
    return boost::geometry::area(p);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename Poly, typename Point>
CINO_INLINE
bool polygon_contains(const Poly & poly, const Point & point, const bool border_counts)
{
    double x = point.x();
    double y = point.y();

    if(border_counts) return boost::geometry::covered_by(BoostPoint(x,y), poly);
                      return boost::geometry::within    (BoostPoint(x,y), poly);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename Poly0, typename Poly1>
CINO_INLINE
BoostMultiPolygon polygon_union(const Poly0 & p0, const Poly1 & p1)
{
    BoostMultiPolygon res;
    boost::geometry::union_(p0, p1, res);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename Poly0, typename Poly1>
CINO_INLINE
BoostMultiPolygon polygon_difference(const Poly0 & p0, const Poly1 & p1)
{
    BoostMultiPolygon res;
    boost::geometry::difference(p0, p1, res);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename Poly0, typename Poly1>
CINO_INLINE
BoostMultiPolygon polygon_intersection(const Poly0 & p0, const Poly1 & p1)
{
    BoostMultiPolygon res;
    boost::geometry::intersection(p0, p1, res);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void polygon_get_edges(const std::vector<BoostPoint> & poly,
                             std::vector<vec2d>      & verts,
                             std::vector<uint>       & edges)
{
    uint base = verts.size();
    uint nv   = poly.size()-1; // first and last verts coincide...
    for(uint vid=0; vid<nv; ++vid)
    {
        verts.push_back(vec2d(boost::geometry::get<0>(poly.at(vid)),
                              boost::geometry::get<1>(poly.at(vid))));
        edges.push_back(base + vid);
        edges.push_back(base + (vid+1)%nv);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void polygon_get_edges(const BoostPolygon       & poly,
                             std::vector<vec2d> & verts,
                             std::vector<uint>  & edges)
{
    polygon_get_edges(poly.outer(), verts, edges);
    for(auto hole : poly.inners()) polygon_get_edges(hole, verts, edges);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void polygon_get_edges(const BoostMultiPolygon  & poly,
                             std::vector<vec2d> & verts,
                             std::vector<uint>  & edges)
{
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

CINO_INLINE
void triangulate_polygon(const std::vector<BoostPoint> & poly,
                         const std::string               flags,
                               std::vector<vec2d>      & verts,
                               std::vector<uint>       & tris)
{
    std::vector<vec2d> v, h;
    std::vector<uint>  e;
    polygon_get_edges(poly, v, e);
    triangle_wrap(v, e, h, flags, verts, tris);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void triangulate_polygon(const BoostPolygon            & poly,
                         const std::string               flags,
                               std::vector<vec2d>      & verts,
                               std::vector<uint>       & tris)
{
    // find one seed per hole (to robustly clear holes from triangulation)
    std::vector<vec2d> h_seeds;
    for(uint hid=0; hid<poly.inners().size(); ++hid)
    {
        std::vector<vec2d> v_in, h, v_out;
        std::vector<uint>  e, t;
        polygon_get_edges(poly.inners().at(hid), v_in, e);
        triangle_wrap(v_in, e, h, "Q", v_out, t);
        uint v0 = t.at(0);
        uint v1 = t.at(1);
        uint v2 = t.at(2);
        h_seeds.push_back((v_out.at(v0)+v_out.at(v1)+v_out.at(v2))/3.0);
    }

    std::vector<vec2d> v;
    std::vector<uint>  e;
    polygon_get_edges(poly, v, e);
    triangle_wrap(v, e, h_seeds, flags, verts, tris);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void triangulate_polygon(const BoostMultiPolygon       & poly,
                         const std::string               flags,
                               std::vector<vec2d>      & verts,
                               std::vector<uint>       & tris)
{
    // find one seed per hole (to robustly clear holes from triangulation)
    std::vector<vec2d> h_seeds;
    for(const BoostPolygon & p : poly)
    {
        for(uint hid=0; hid<p.inners().size(); ++hid)
        {
            std::vector<vec2d> v_in, h, v_out;
            std::vector<uint>  e, t;
            polygon_get_edges(p.inners().at(hid), v_in, e);
            triangle_wrap(v_in, e, h, "Q", v_out, t);
            uint v0 = t.at(0);
            uint v1 = t.at(1);
            uint v2 = t.at(2);
            h_seeds.push_back((v_out.at(v0)+v_out.at(v1)+v_out.at(v2))/3.0);
        }
    }

    std::vector<vec2d> v;
    std::vector<uint>  e;
    polygon_get_edges(poly, v, e);
    triangle_wrap(v, e, h_seeds, flags, verts, tris);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void triangulate_polygon(const std::vector<BoostPoint> & poly,
                         const std::string               flags,
                         const double                  & z, // adds third coordinate
                               std::vector<vec3d>      & verts,
                               std::vector<uint>       & tris)
{
    std::vector<vec2d> v;
    triangulate_polygon(poly, flags, v, tris);
    verts = vec3d_from_vec2d(v, z);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void triangulate_polygon(const BoostPolygon            & poly,
                         const std::string               flags,
                         const double                  & z, // adds third coordinate
                               std::vector<vec3d>      & verts,
                               std::vector<uint>       & tris)
{
    std::vector<vec2d> v;
    triangulate_polygon(poly, flags, v, tris);
    verts = vec3d_from_vec2d(v, z);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void triangulate_polygon(const BoostMultiPolygon       & poly,
                         const std::string               flags,
                         const double                  & z, // adds third coordinate
                               std::vector<vec3d>      & verts,
                               std::vector<uint>       & tris)
{
    std::vector<vec2d> v;
    triangulate_polygon(poly, flags, v, tris);
    verts = vec3d_from_vec2d(v, z);
}

}
