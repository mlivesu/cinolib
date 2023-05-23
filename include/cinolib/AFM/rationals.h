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
#ifndef CINO_AFM_RATIONALS_H
#define CINO_AFM_RATIONALS_H

#ifdef CINOLIB_USES_CGAL

#include <CGAL/Lazy_exact_nt.h>
#include <CGAL/Gmpq.h>
#include <cinolib/cino_inline.h>

namespace cinolib
{


typedef CGAL::Lazy_exact_nt<CGAL::Gmpq> CGAL_Q;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
CGAL_Q orient3d(const CGAL_Q * pa,
                const CGAL_Q * pb,
                const CGAL_Q * pc,
                const CGAL_Q * pd);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
CGAL_Q orient2d(const CGAL_Q * pa,
                const CGAL_Q * pb,
                const CGAL_Q * pc);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void midpoint(const CGAL_Q * pa,
              const CGAL_Q * pb,
              const CGAL_Q * pc,
                    CGAL_Q * res); // res = (pa + pb + pc)/3

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void midpoint(const CGAL_Q * pa,
              const CGAL_Q * pb,
                    CGAL_Q * res); // res = (pa + pb)/2

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
CGAL_Q sqrd_distance2d(const CGAL_Q * pa,
                       const CGAL_Q * pb);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void copy(const CGAL_Q * pa, CGAL_Q * pb);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void line_intersection2d(const CGAL_Q * pa,
                         const CGAL_Q * pb,
                         const CGAL_Q * pc,
                         const CGAL_Q * pd,
                               CGAL_Q * res); // intersection point of **NON PARALLEL** lines (pa,pb) and (pc,pd)

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool rationals_are_working();

}

#ifndef  CINO_STATIC_LIB
#include "rationals.cpp"
#endif

#endif // CINOLIB_USES_CGAL
#endif // CINO_AFM_RATIONALS_H
