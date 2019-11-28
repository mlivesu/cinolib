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
*     Daniela Cabiddu                                                           *
*     http://www.imati.cnr.it/index.php/people/8-curricula/119-daniela-cabiddu  *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#include <cinolib/polygon_maximum_inscribed_circle.h>
#include <cinolib/geometry/segment.h>
#include <cinolib/min_max_inf.h>

// Most of this is coming from here:
// http://www.boost.org/doc/libs/1_65_1/libs/polygon/doc/voronoi_diagram.htm
// http://www.boost.org/doc/libs/1_65_0/libs/polygon/example/voronoi_basic_tutorial.cpp
//
#ifdef CINOLIB_USES_BOOST
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry.hpp>
#include <boost/polygon/voronoi.hpp>
//
using boost::polygon::voronoi_builder;
using boost::polygon::voronoi_diagram;
using boost::polygon::voronoi_edge;
using boost::polygon::x;
using boost::polygon::y;
using boost::polygon::low;
using boost::polygon::high;
//
struct polygon_point
{
    int x;
    int y;
    polygon_point(int x, int y) : x(x), y(y) {}
};
//
struct polygon_segment
{
    polygon_point p0;
    polygon_point p1;
    polygon_segment(int x1, int y1, int x2, int y2) : p0(x1, y1), p1(x2, y2) {}
};
//
template<>
struct boost::polygon::geometry_concept<polygon_point>
{
    typedef boost::polygon::point_concept type;
};
//
template<>
struct boost::polygon::point_traits<polygon_point>
{
    typedef int coordinate_type;
    static inline coordinate_type get(const polygon_point & point, orientation_2d orient)
    {
        return (orient == HORIZONTAL) ? point.x : point.y;
    }
};
//
template<>
struct boost::polygon::geometry_concept<polygon_segment>
{
    typedef boost::polygon::segment_concept type;
};
//
template<>
struct boost::polygon::segment_traits<polygon_segment>
{
    typedef int coordinate_type;
    typedef polygon_point point_type;

    static inline point_type get(const polygon_segment& segment, direction_1d dir)
    {
        return dir.to_int() ? segment.p1 : segment.p0;
    }
};
//
typedef boost::geometry::model::d2::point_xy<double> BoostPoint;
typedef boost::geometry::model::polygon<BoostPoint>  BoostPolygon;

#endif // CINOLIB_USES_BOOST

namespace cinolib
{

#ifdef CINOLIB_USES_BOOST

BoostPolygon make_boost_poly(const std::vector<vec2d> & poly)
{
    BoostPolygon boost_poly;
    for(vec2d p : poly) boost::geometry::append(boost_poly, BoostPoint(p.x(), p.y()));
    boost::geometry::correct(boost_poly);
    return boost_poly;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void polygon_maximum_inscribed_circle(const std::vector<vec2d> & poly,
                                            vec2d              & center,
                                            double             & radius)
{
    radius = 0.0;
    center = vec2d(0,0);

    vec2d min( inf_double,  inf_double);
    vec2d max(-inf_double, -inf_double);
    for(auto p : poly)
    {
        min = min.min(p);
        max = max.max(p);
    }

    double scale_factor;
    double diag = min.dist(max);

    if(diag >= 1.0) scale_factor = std::numeric_limits<int>::max() / diag;
    else            scale_factor = std::numeric_limits<int>::max() * diag;

    scale_factor *= 0.5; // dumb attempt to stay on the safe side and avoid overflows...

    // Boost implementation of Generazlied Voronoi uses integer coordinates.
    // In order to achieve maximum precision I am scaling the polygon as much
    // as I can in order to minimize loss of precision during integer roundoff...

    std::vector<polygon_segment> segments;
    for(uint i=0; i<poly.size(); ++i)
    {
        vec2d v0 = scale_factor * poly.at(i);
        vec2d v1 = scale_factor * poly.at((i+1)%poly.size());
        segments.push_back(polygon_segment(v0.x(), v0.y(), v1.x(), v1.y()));
    }

    voronoi_diagram<double> vd;
    construct_voronoi(segments.begin(), segments.end(), &vd);

    BoostPolygon boost_poly = make_boost_poly(poly);
    for(auto it=vd.vertices().begin(); it!=vd.vertices().end(); ++it)
    {
        const voronoi_diagram<double>::vertex_type &v = *it;
        const voronoi_diagram<double>::edge_type   *e = v.incident_edge();
        const voronoi_diagram<double>::cell_type   *c = e->cell();
        const polygon_segment                        &s = segments.at(c->source_index());

        // do not consider Voronoi vertices outside the polygon
        if (boost::geometry::within(BoostPoint(v.x()/scale_factor, v.y()/scale_factor), boost_poly))
        {
            // annoying wrap to vec3d (TODO: template cinolib::Segment to make it work in 2D too)
            vec3d beg(s.p0.x, s.p0.y, 0);
            vec3d end(s.p1.x, s.p1.y, 0);
            vec3d c3d(v.x(),  v.y(),  0);
            cinolib::Segment tmp(0, beg,end);
            double d = tmp.dist(c3d);

            if(d>radius)
            {
                radius = d;
                center = vec2d(c3d); // will automatically drop z
            }
        }
    }

    radius /= scale_factor;
    center /= scale_factor;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void polygon_maximum_inscribed_circle(const std::vector<vec3d> & poly,   // will drop z component
                                            vec3d              & center, // will have z=0
                                            double             & radius)
{
    std::vector<vec2d> poly_2d;
    for(auto p : poly) poly_2d.push_back(vec2d(p.x(), p.y()));

    vec2d center_2d;
    polygon_maximum_inscribed_circle(poly_2d, center_2d, radius);
    center = vec3d(center_2d.x(), center_2d.y(), 0);
}

#endif // CINOLIB_USES_BOOST

}
