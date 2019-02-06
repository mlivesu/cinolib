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
#ifndef CINO_BOOST_POLYGON_WRAP_H
#define CINO_BOOST_POLYGON_WRAP_H

#ifdef CINOLIB_USES_BOOST

#include <cinolib/geometry/vec2.h>

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
    // any vec type is fine, as long as it provides Point::x() and Point::y()

    template<typename Point>
    CINO_INLINE
    BoostPolygon make_polygon(const std::vector<Point> & outer_ring);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    template<typename Point>
    CINO_INLINE
    BoostPolygon make_polygon(const std::vector<Point>              & outer_ring,
                              const std::vector<std::vector<Point>> & inner_rings);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    template<typename Point>
    CINO_INLINE
    BoostPolygon make_polygon(const std::vector<Point> & polyline,
                              const double             thickening_radius);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    template<typename Poly>
    CINO_INLINE
    Poly polygon_simplify(const Poly & p, const double max_dist);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    template<typename Poly>
    CINO_INLINE
    double polygon_area(const Poly & p);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    template<typename Poly, typename Point>
    CINO_INLINE
    bool polygon_contains(const Poly & poly, const Point & point, const bool border_counts);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    template<typename Poly0, typename Poly1>
    CINO_INLINE
    BoostMultiPolygon polygon_union(const Poly0 & p0, const Poly1 & p1);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    template<typename Poly0, typename Poly1>
    CINO_INLINE
    BoostMultiPolygon polygon_difference(const Poly0 & p0, const Poly1 & p1);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    template<typename Poly0, typename Poly1>
    CINO_INLINE
    BoostMultiPolygon polygon_intersection(const Poly0 & p0, const Poly1 & p1);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    CINO_INLINE
    void polygon_get_edges(const std::vector<BoostPoint> & poly,
                                 std::vector<vec2d>      & verts,
                                 std::vector<uint>       & edges);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    CINO_INLINE
    void polygon_get_edges(const BoostPolygon       & poly,
                                 std::vector<vec2d> & verts,
                                 std::vector<uint>  & edges);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    CINO_INLINE
    void polygon_get_edges(const BoostMultiPolygon  & poly,
                                 std::vector<vec2d> & verts,
                                 std::vector<uint>  & edges);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    CINO_INLINE
    void polygon_get_edges(const BoostPolygon       & poly,
                           const double             & z, // adds third coordinate
                                 std::vector<vec3d> & verts,
                                 std::vector<uint>  & edges);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    CINO_INLINE
    void polygon_get_edges(const BoostMultiPolygon  & poly,
                           const double             & z, // adds third coordinate
                                 std::vector<vec3d> & verts,
                                 std::vector<uint>  & edges);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    CINO_INLINE
    void triangulate_polygon(const std::vector<BoostPoint> & poly,
                             const std::string               flags,
                                   std::vector<vec2d>      & verts,
                                   std::vector<uint>       & tris);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    CINO_INLINE
    void triangulate_polygon(const BoostPolygon            & poly,
                             const std::string               flags,
                                   std::vector<vec2d>      & verts,
                                   std::vector<uint>       & tris);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    CINO_INLINE
    void triangulate_polygon(const BoostMultiPolygon       & poly,
                             const std::string               flags,
                                   std::vector<vec2d>      & verts,
                                   std::vector<uint>       & tris);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    CINO_INLINE
    void triangulate_polygon(const std::vector<BoostPoint> & poly,
                             const std::string               flags,
                             const double                  & z, // adds third coordinate
                                   std::vector<vec3d>      & verts,
                                   std::vector<uint>       & tris);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    CINO_INLINE
    void triangulate_polygon(const BoostPolygon            & poly,
                             const std::string               flags,
                             const double                  & z, // adds third coordinate
                                   std::vector<vec3d>      & verts,
                                   std::vector<uint>       & tris);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    CINO_INLINE
    void triangulate_polygon(const BoostMultiPolygon       & poly,
                             const std::string               flags,
                             const double                  & z, // adds third coordinate
                                   std::vector<vec3d>      & verts,
                                   std::vector<uint>       & tris);
}

#ifndef  CINO_STATIC_LIB
#include "boost_polygon_wrap.cpp"
#endif

#endif // CINOLIB_USES_BOOST
#endif // CINO_BOOST_POLYGON_WRAP_H
