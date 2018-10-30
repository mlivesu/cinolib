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
#ifndef POLYGON2D_H
#define POLYGON2D_H

#include <vector>
#include <ostream>
#include <sys/types.h>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/linestring.hpp>

namespace cinolib
{

typedef boost::geometry::model::d2::point_xy<double>        BoostPoint;
typedef boost::geometry::model::polygon<BoostPoint>         BoostPolygon;
typedef boost::geometry::model::multi_polygon<BoostPolygon> BoostMultiPolygon;
typedef boost::geometry::model::linestring<BoostPoint>      BoostLinestring;

class Polygon2D
{
    public:

        explicit Polygon2D();

        explicit Polygon2D(const BoostPolygon & p);

        explicit Polygon2D(const std::vector<double> & ring);

        explicit Polygon2D(const std::vector<double>              & outer_ring,
                           const std::vector<std::vector<double>> & inner_rings);

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint n_vertices() const;
        uint  n_out_vertices() const;
        uint  n_holes() const;
        uint  n_hole_vertices() const;
        uint  n_hole_vertices(const uint hid) const;

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::vector<Polygon2D> operator & (const Polygon2D & p) const; // and
        std::vector<Polygon2D> operator | (const Polygon2D & p) const; // or
        std::vector<Polygon2D> operator ^ (const Polygon2D & p) const; // xor
        std::vector<Polygon2D> operator - (const Polygon2D & p) const; // minus
        std::vector<Polygon2D> overlay    (const Polygon2D & p) const; // (A-B) U (A&B)
        std::vector<Polygon2D> sym_overlay(const Polygon2D & p) const; // (A^B) U (A&B)

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void simplify(const double max_dist);
        void remove_duplicated_vertices();
        void merge_adjacent_nearby_vertices(const double eps);
        void merge_collinear_edges(const double angle_deg);

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void set_coords(const uint ring, const uint pos, const double x, const double y);
        void get_coords(const uint ring, const uint pos, double & x, double & y) const;

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double min_x() const;
        double min_y() const;
        double max_x() const;
        double max_y() const;

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool   is_inside(const Polygon2D & p, bool borders_count = true) const;
        bool   is_inside(const double x, const double y, bool borders_count = true) const;
        bool   intersects(const Polygon2D & p, bool borders_count = true) const;
        void   get_a_point_inside(double & x, double & y) const;

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void   triangulate(std::vector<double> & coords,
                           std::vector<uint>   & tris,
                           const double          z_coord) const;

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        BoostPolygon poly;
};

}

#ifndef  CINO_STATIC_LIB
#include "polygon2d.cpp"
#endif

#endif // POLYGON2D_H
