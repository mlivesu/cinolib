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
#include <cinolib/geometry/polygon.h>
#include <cinolib/geometry/plane.h>
#include <cinolib/geometry/triangle.h>
#include <cinolib/Shewchuk_predicates.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// http://mathworld.wolfram.com/PolygonArea.html
CINO_INLINE
double polygon_signed_area(const std::vector<vec2d> & poly)
{
    double area = 0.0;
    for(uint i=0; i<poly.size(); ++i)
    {
        const vec2d & a = poly.at(i);
        const vec2d & b = poly.at((i+1)%poly.size());

        area += a.x()*b.y() - a.y()*b.x();
    }
    return area * 0.5;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double polygon_unsigned_area(const std::vector<vec2d> & poly)
{
    return std::fabs(polygon_signed_area(poly));
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double polygon_is_CCW(const std::vector<vec2d> & poly)
{
    return (polygon_signed_area(poly) > 0);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d polygon_normal(const std::vector<vec3d> & poly)
{
    Plane  best_fit(poly);
    vec3d  Z     = vec3d(0,0,1);
    vec3d  axis  = best_fit.n.cross(Z);
    double angle = best_fit.n.angle_rad(Z);

    std::vector<vec2d> poly2d;
    for(auto p : poly)
    {
        vec3d tmp = best_fit.project_onto(p);
        rotate(tmp, axis, angle);
        poly2d.push_back(vec2d(tmp)); // will drop z
    }

    if (polygon_is_CCW(poly2d)) return best_fit.n;
    return -best_fit.n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// project the polygon onto its best fitting plane,
// align with XY, and drop Z coordinate
//
// NOTE: flattening does not preserve winding!
//
CINO_INLINE
void polygon_flatten(const std::vector<vec3d> & poly3d,
                           std::vector<vec2d> & poly2d)
{
    poly2d.clear();
    poly2d.reserve(poly3d.size());

    Plane  best_fit(poly3d);
    vec3d  Z     = vec3d(0,0,1);
    vec3d  axis  = best_fit.n.cross(Z);
    double angle = best_fit.n.angle_rad(Z);

    for(auto p : poly3d)
    {
        vec3d tmp = best_fit.project_onto(p);
        rotate(tmp, axis, angle);
        poly2d.push_back(vec2d(tmp)); // will drop z
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// This is the fundamental building block of the ear cut algorithm for polygon triangulation
// http://cgm.cs.mcgill.ca/~godfried/teaching/cg-projects/97/Ian/algorithm1.html
//
CINO_INLINE
int polygon_find_ear(const std::vector<vec2d> & poly)
{
    uint curr = 0;
    bool ear_not_found = true;
    while(ear_not_found)
    {
        uint prev = (curr+poly.size()-1)%poly.size();
        uint next = (curr+1)%poly.size();

        vec2d A = poly.at(prev);
        vec2d B = poly.at(curr);
        vec2d C = poly.at(next);

        if (orient2d(A, B, C) > 0) // left turn => convex corner
        {
            bool contains_other_point = false;
            for(uint j=0; j<poly.size(); ++j)
            {
                if (j == curr || j == prev || j == next) continue;
                if (triangle_point_is_inside(A, B, C, poly.at(j)))
                {
                    contains_other_point = true;
                }
            }
            if (!contains_other_point) ear_not_found = false;
        }

        if (ear_not_found) ++curr;
        if (curr >= poly.size()) return -1; // no ear could be found (usually means the polygon is degenerate)
    }
    return curr;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Implementation of the ear-cut triangulation algorithm
//
CINO_INLINE
bool polygon_triangulate(std::vector<vec2d> & poly,
                         std::vector<uint>  & tris)
{
    tris.clear();

    // If the polygon is not CCW, flip it along X
    //
    if (!polygon_is_CCW(poly))
    {
        for(auto & p : poly) p.x() = -p.x();
    }

    std::map<uint,uint> v_map;
    for(uint vid=0; vid<poly.size(); ++vid) v_map[vid] = vid;

    std::vector<vec2d> sub_poly = poly;
    while(sub_poly.size() >= 3)
    {
        int curr = polygon_find_ear(sub_poly);
        if (curr<0)
        {
            std::cerr << "WARNING: ear cut algorithm failure (is the polygon degenerate?)" << std::endl;
            tris.clear();
            return false;
        }
        uint prev = (curr+sub_poly.size()-1)%sub_poly.size();
        uint next = (curr+1)%sub_poly.size();

        tris.push_back(v_map.at(prev));
        tris.push_back(v_map.at(curr));
        tris.push_back(v_map.at(next));

        // update poly and v_map
        std::map<uint,uint> tmp_v_map;
        std::vector<vec2d>  tmp_poly;
        for(uint vid=0; vid<sub_poly.size(); ++vid)
        {
            if ((int)vid==curr) continue;
            tmp_poly.push_back(sub_poly.at(vid));
            tmp_v_map[tmp_poly.size()-1] = v_map.at(vid);
        }
        sub_poly = tmp_poly;
        v_map    = tmp_v_map;
    }

    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool polygon_triangulate(std::vector<vec3d> &poly,
                         std::vector<uint> & tris)
{
    std::vector<vec2d> poly2d;
    polygon_flatten(poly, poly2d);
    return polygon_triangulate(poly2d, tris);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double polygon_is_convex(const std::vector<vec2d> & poly)
{
    bool turn_left  = false;
    bool turn_right = false;
    for(uint curr=0; curr<poly.size(); ++curr)
    {
        uint prev = (curr>0) ? curr-1 : poly.size()-1;
        uint next = (curr<poly.size()-1) ? curr+1 : 0;

        double sign = orient2d(poly.at(prev), poly.at(curr), poly.at(next));

        if (sign > 0) turn_left  = true; else
        if (sign < 0) turn_right = true;

        if (turn_left && turn_right) return false;
    }
    return true;
}

}

