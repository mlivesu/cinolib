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
