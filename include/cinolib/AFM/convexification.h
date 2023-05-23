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
#ifndef CINO_AFM_CONVEXIFICATION_H
#define CINO_AFM_CONVEXIFICATION_H

#include <cinolib/AFM/AFM.h>

namespace cinolib
{

CINO_INLINE
void convexify_front(AFM_data & data,
                     const uint v0,
                     const uint v1,
                     const uint v2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// checks whether the positive half space of the edge of pid that is opposite
// to vid contains the point p. This is relevant to "push" vertex
// vid towards the origin, in order to convexify a concave portion of the front
// and unlock an advancemente move via edge flip
//
CINO_INLINE
bool triangle_is_blocking(AFM_data     & data,
                          const uint     vid,
                          const uint     pid,
                          const CGAL_Q * p);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint vertex_locks(AFM_data         & data,
                  const uint          vid,
                  std::vector<uint> & locks,
                  const CGAL_Q      * p);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void vertex_unlock(AFM_data & data,
                   const uint     vid,
                   const CGAL_Q * p);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void update_vertex_pos(AFM_data & data, const uint vid, const CGAL_Q * p);

}

#ifndef  CINO_STATIC_LIB
#include "convexification.cpp"
#endif

#endif // CINO_AFM_CONVEXIFICATION_H
