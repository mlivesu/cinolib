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
#include <cinolib/geometry/triangle_utils.h>
#include <cinolib/standard_elements_tables.h>
#include <cinolib/min_max_inf.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d triangle_normal(const vec3d A, const vec3d B, const vec3d C)
{
    vec3d n = (B-A).cross(C-A);
    if(!n.is_null()) n.normalize();
    return n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class vec>
CINO_INLINE
double triangle_area(const vec A, const vec B, const vec C)
{
    return (0.5 * (B-A).cross(C-A).length());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Given a triangle t(A,B,C) and a ray r(P,dir) compute both
// the edge and position where r exits from t
//
// NOTE: r is assumed to live "within" t, like in a gradient field for example...
//
CINO_INLINE
void triangle_traverse_with_ray(const vec3d   tri[3],
                                const vec3d   P,
                                const vec3d   dir,
                                      vec3d & exit_pos,
                                      uint   & exit_edge)
{
    // 1) Find the exit edge
    //

    vec3d uvw[3] = { tri[0]-P, tri[1]-P, tri[2]-P };

    std::set< std::pair<double,int> > sorted_by_angle;
    for(uint i=0; i<3; ++i)
    {
        sorted_by_angle.insert(std::make_pair(dir.angle_rad(uvw[i]),i));
    }

    uint  vert  = (*sorted_by_angle.begin()).second;
    vec3d tn    = triangle_normal(tri[0], tri[1], tri[2]);
    vec3d cross = dir.cross(uvw[vert]);

    exit_edge = (cross.dot(tn) >= 0) ? (vert+2)%3 : vert; // it's because e_i = ( v_i, v_(i+1)%3 )

    //
    // 2) Find the exit point using the law of sines
    //

    vec3d  app     = tri[TRI_EDGES[exit_edge][1]];
    vec3d  V1      = tri[TRI_EDGES[exit_edge][0]];
    vec3d  V0      = P;
    vec3d  e0_dir  = app - V1; e0_dir.normalize();
    vec3d  e1_dir  = -dir;
    vec3d  e2_dir  = V1 - V0; e2_dir.normalize();

    double V0_ang  = e2_dir.angle_rad(-e1_dir);
    double V2_ang  = e1_dir.angle_rad(-e0_dir);
    double e2_len  = (V1 - V0).length();
    double e0_len  = triangle_law_of_sines(V2_ang, V0_ang, e2_len);

    exit_pos =  V1 + e0_len * e0_dir;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// https://en.wikipedia.org/wiki/Law_of_sines
//
CINO_INLINE
double triangle_law_of_sines(const double angle_0, const double angle_1, const double length_0) // returns length_1
{
    return sin(angle_1) * length_0 / sin(angle_0);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// http://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
//
// NOTE: the current implementation requires 21 multiplications and 2 divisions.
// A good alternative could be the method proposed in:
//
//   Computing the Barycentric Coordinates of a Projected Point
//   Wolfgang Heidrich
//   Journal of Graphics, GPU, and Game Tools, 2011
//
// which takes 27 multiplications (3 cross, 3 dot products), and
// combines together projection of the point in the triangle's plane
// and computation of barycentric coordinates
//
template <class vec>
CINO_INLINE
void triangle_barycentric_coords(const vec & A,
                                 const vec & B,
                                 const vec & C,
                                 const vec & P,
                                 double wgts[])
{
    vec    u    = B - A;
    vec    v    = C - A;
    vec    w    = P - A;
    double d00  = u.dot(u);
    double d01  = u.dot(v);
    double d11  = v.dot(v);
    double d20  = w.dot(u);
    double d21  = w.dot(v);
    double den  = d00 * d11 - d01 * d01;

    if(den==0) // degenerate
    {
        wgts[0] = inf_double;
        wgts[1] = inf_double;
        wgts[2] = inf_double;
        return;
    }

    wgts[2] = (d00 * d21 - d01 * d20) / den; assert(!std::isnan(wgts[2]));
    wgts[1] = (d11 * d20 - d01 * d21) / den; assert(!std::isnan(wgts[1]));
    wgts[0] = 1.0f - wgts[1] - wgts[2];      assert(!std::isnan(wgts[0]));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Given a point P and a triangle ABC, finds the point in ABC that
// is closest to P. This code was taken directly from Ericson's
// seminal book "Real Time Collision Detection", Section 5.1.5
//
CINO_INLINE
vec3d triangle_closest_point(const vec3d & P,
                             const vec3d & A,
                             const vec3d & B,
                             const vec3d & C)
{
    vec3d AB = B-A;
    vec3d AC = C-A;
    vec3d BC = C-B;

    // compute parameter s for projection P’ of P on AB,
    // P’ = A+s*AB, s = s_num/(s_num+s_den)
    double s_num = (P-A).dot(AB);
    double s_den = (P-B).dot(A-B);

    // compute parameter t for projection P’ of P on AC,
    // P’ = A+t*AC, s = t_num/(t_num+t_den)
    double t_num = (P-A).dot(AC);
    double t_den = (P-C).dot(A-C);

    if(s_num<=0.0 && t_num<=0.0) return A;

    // compute parameter u for projection P’ of P on BC,
    // P’ = B+u*BC, u = u_num/(u_num+u_den)
    double u_num = (P-B).dot(BC);
    double u_den = (P-C).dot(B-C);

    if(s_den<=0.0 && u_num<=0.0) return B;
    if(t_den<=0.0 && u_den<=0.0) return C;

    // P is outside (or on) AB if the triple scalar product [N PA PB]<=0
    vec3d  n  = (B-A).cross(C-A);
    double vc = n.dot((A-P).cross(B-P));

    // If P outside AB and within feature region of AB, // return projection of P onto AB
    if(vc<=0.0 && s_num>=0.0 && s_den>=0.0)
    {
        return A + s_num/(s_num+s_den)*AB;
    }

    // P is outside (or on) BC if the triple scalar product [N PB PC]<=0
    double va = n.dot((B-P).cross(C-P));

    // If P outside BC and within feature region of BC, // return projection of P onto BC
    if(va<=0.0 && u_num>=0.0 && u_den>=0.0)
    {
        return B + u_num/(u_num+u_den)*BC;
    }

    // P is outside (or on) CA if the triple scalar product [N PC PA]<=0
    double vb = n.dot((C-P).cross(A-P));

    // If P outside CA and within feature region of CA, // return projection of P onto CA
    if(vb<=0.0 && t_num>=0.0 && t_den>=0.0)
    {
        return A + t_num/(t_num+t_den)*AC;
    }

    // P must project inside face region. Compute Q using barycentric coordinates
    double u = va / (va + vb + vc);
    double v = vb / (va + vb + vc);
    double w = 1.0 - u - v;
    return u*A + v*B + w*C;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double point_to_triangle_dist(const vec3d & P,
                              const vec3d & A,
                              const vec3d & B,
                              const vec3d & C)
{
    return P.dist(triangle_closest_point(P,A,B,C));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double point_to_triangle_dist_sqrd(const vec3d & P,
                                   const vec3d & A,
                                   const vec3d & B,
                                   const vec3d & C)
{
    return P.dist_squared(triangle_closest_point(P,A,B,C));
}

}

