/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2022: Marco Livesu                                              *
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
#include <cinolib/geometry/segment_utils.h>
#include <cinolib/clamp.h>

namespace cinolib
{

/* Given two segments S0(s00,s01) and S1(s10,s11) finds the pair
 * of points in S0,S1 that are closest to each other. The function
 * returns the distance between such points. Parameters p_on_s0,
 * p_on_s1, s and t allow to precisely locate points on each segment,
 * both in explicit and parametric form. In case the two segments are
 * intersecting p_on_s0 and p_on_s1 coincide.
 *
 *      Ref: Real Time Collision Detection, Section 5.1.9
*/
template<class T>
CINO_INLINE
T closest_points_between_segments(const mat<3,1,T> & s00,
                                  const mat<3,1,T> & s01,
                                  const mat<3,1,T> & s10,
                                  const mat<3,1,T> & s11,
                                        T          & s,
                                        T          & t,
                                        mat<3,1,T> & p_on_s0,
                                        mat<3,1,T> & p_on_s1)
{
    mat<3,1,T> d1  = s01 - s00;  // direction of segment s0
    mat<3,1,T> d2  = s11 - s10;  // direction of segment s1
    mat<3,1,T> r   = s00 - s10;
    T          a   = d1.dot(d1); // squared length of s0
    T          e   = d2.dot(d2); // squared length of s1
    T          f   = d2.dot(r);
    T          EPS = 1e-10;      // segments shorter than EPS will be considered zero length

    // check if either or both segments degenerate into points
    if(a<=EPS && e<=EPS)
    {
        // both segments degenerate into points
        s = 0.0;
        t = 0.0;
        p_on_s0 = s00;
        p_on_s1 = s10;
        return (T)p_on_s0.dist(p_on_s1);
    }
    if(a<=EPS)
    {
        // first segment degenerates into a point
        s = 0.0;
        t = clamp(f/e,(T)0,(T)1);
    }
    else
    {
        T c = d1.dot(r);
        if(e<=EPS)
        {
            // second segment degenerates into a point
            s = clamp(-c/a,(T)0,(T)1);
            t = 0;
        }
        else // the general non degenerate case starts here
        {
            T b   = d1.dot(d2);
            T den = a*e-b*b; // always non negative

            s = (den!=(T)0) ? clamp((b*f-c*e)/den, (T)0, (T)1) : (T)0;
            t = (b*s+f)/e;

            if(t<(T)0)
            {
                t = (T)0;
                s = clamp(-c/a, (T)0, (T)1);
            }
            else if(t > 1.0)
            {
                t = 1.0;
                s = clamp((b-c)/a, (T)0, (T)1);
            }
        }
    }

    p_on_s0 = s00 + d1 * s;
    p_on_s1 = s10 + d2 * t;
    return (T)p_on_s0.dist(p_on_s1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/* WARNING: this assumes that the segments DO INTERSECT!
 * For exact and approximate intersection checks, please
 * refer to cinolib/predicates.h
*/
template<class T>
CINO_INLINE
mat<3,1,T> segment_intersection(const mat<3,1,T> & s00,
                                const mat<3,1,T> & s01,
                                const mat<3,1,T> & s10,
                                const mat<3,1,T> & s11)
{
    mat<3,1,T> p;
    T s,t;
    closest_points_between_segments(s00,s01,s10,s11,s,t,p,p);
    return p;
}

}
