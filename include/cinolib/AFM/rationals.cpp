/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2023: Marco Livesu                                              *
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
#include <cinolib/AFM/rationals.h>
#include <cinolib/predicates.h>

namespace cinolib
{

CINO_INLINE
bool rationals_are_working()
{
    CGAL_Q a[3] = { 0,0,0 };
    CGAL_Q b[3] = { 1,2,3 };
    CGAL_Q c[3] = { 9,3,1 };
    CGAL_Q d[3];
    midpoint(a,b,c,d);

    double aa[3] = { 0,0,0 };
    double bb[3] = { 1,2,3 };
    double cc[3] = { 9,3,1 };
    double dd[3] = { (aa[0]+bb[0]+cc[0])/3,
                     (aa[1]+bb[1]+cc[1])/3,
                     (aa[2]+bb[2]+cc[2])/3 };

    if(orient3d(a,b,c,d)==0 && orient3d(aa,bb,cc,dd)!=0)
    {
        //std::cout << "CGAL::Lazy_exact_nt<CGAL::Gmpq> CHECK ... OK" << std::endl;
        return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
CGAL_Q orient3d(const CGAL_Q * pa,
                const CGAL_Q * pb,
                const CGAL_Q * pc,
                const CGAL_Q * pd)
{
    CGAL_Q adx = pa[0] - pd[0];
    CGAL_Q bdx = pb[0] - pd[0];
    CGAL_Q cdx = pc[0] - pd[0];
    CGAL_Q ady = pa[1] - pd[1];
    CGAL_Q bdy = pb[1] - pd[1];
    CGAL_Q cdy = pc[1] - pd[1];
    CGAL_Q adz = pa[2] - pd[2];
    CGAL_Q bdz = pb[2] - pd[2];
    CGAL_Q cdz = pc[2] - pd[2];

    return adx * (bdy * cdz - bdz * cdy)
         + bdx * (cdy * adz - cdz * ady)
         + cdx * (ady * bdz - adz * bdy);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
CGAL_Q orient2d(const CGAL_Q * pa,
                const CGAL_Q * pb,
                const CGAL_Q * pc)
{
    CGAL_Q acx = pa[0] - pc[0];
    CGAL_Q bcx = pb[0] - pc[0];
    CGAL_Q acy = pa[1] - pc[1];
    CGAL_Q bcy = pb[1] - pc[1];

    return acx * bcy - acy * bcx;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void midpoint(const CGAL_Q * pa,
              const CGAL_Q * pb,
              const CGAL_Q * pc,
                    CGAL_Q * res) // res = (pa + pb + pc)/3
{
    res[0] = (pa[0] + pb[0] + pc[0])/3;
    res[1] = (pa[1] + pb[1] + pc[1])/3;
    res[2] = (pa[2] + pb[2] + pc[2])/3;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void midpoint(const CGAL_Q * pa,
              const CGAL_Q * pb,
                    CGAL_Q * res) // res = (pa + pb)/2
{
    res[0] = (pa[0] + pb[0])/2;
    res[1] = (pa[1] + pb[1])/2;
    res[2] = (pa[2] + pb[2])/2;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void line_intersection2d(const CGAL_Q * pa,
                         const CGAL_Q * pb,
                         const CGAL_Q * pc,
                         const CGAL_Q * pd,
                               CGAL_Q * res) // intersection point of **NON PARALLEL** lines (pa,pb) and (pc,pd)
{
    // https://en.wikipedia.org/wiki/Line–line_intersection
    CGAL_Q det_ab   = pa[0]*pb[1] - pa[1]*pb[0];
    CGAL_Q det_cd   = pc[0]*pd[1] - pc[1]*pd[0];
    CGAL_Q det_ab_x = pa[0] - pb[0];
    CGAL_Q det_cd_x = pc[0] - pd[0];
    CGAL_Q det_ab_y = pa[1] - pb[1];
    CGAL_Q det_cd_y = pc[1] - pd[1];

    CGAL_Q den = det_ab_x*det_cd_y - det_ab_y*det_cd_x;
    assert(den!=0);

    res[0] = (det_ab*det_cd_x - det_ab_x*det_cd)/den;
    res[1] = (det_ab*det_cd_y - det_ab_y*det_cd)/den;

    // sanity checks
    assert(orient2d(pa,pb,res)==0);
    assert(orient2d(pc,pd,res)==0);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
CGAL_Q sqrd_distance2d(const CGAL_Q * pa,
                       const CGAL_Q * pb)
{
    return (pa[0]-pb[0])*(pa[0]-pb[0]) +
           (pa[1]-pb[1])*(pa[1]-pb[1]);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void copy(const CGAL_Q * pa, CGAL_Q * pb)
{
    pb[0] = pa[0];
    pb[1] = pa[1];
    pb[2] = pa[2];
}

}





