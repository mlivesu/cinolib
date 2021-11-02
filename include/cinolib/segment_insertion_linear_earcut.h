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
#ifndef CINO_SEGMENT_INSERTION_LINEAR_EARCUT
#define CINO_SEGMENT_INSERTION_LINEAR_EARCUT

#include <sys/types.h>
#include <vector>
#include <cinolib/cino_inline.h>

namespace cinolib
{

/* This routine allows to tessellate simple polygons that arise in the
 * context of constrained triangulation in deterministic linear time
 * using a simplified version of classical earcut, from which we omit any
 * point in triangle test.
 *
 * The method assumes that poly.front() and poly.back() are the endpoints of
 * the constrained segment.
 *
 * For more details refer to:
 *
 *     Deterministic Linear Time Constrained Triangulation using Simplified Earcut
 *     Marco Livesu, Gianmarco Cherchi, Riccardo Scateni, Marco Attene
 *     IEEE Transactions on Visualization and Computer Graphics, 2021
*/

template<class vec>
CINO_INLINE
void segment_insertion_linear_earcut(const std::vector<vec>  & poly,
                                           std::vector<uint> & tris);
}

#ifndef  CINO_STATIC_LIB
#include "segment_insertion_linear_earcut.cpp"
#endif

#endif // CINO_SEGMENT_INSERTION_LINEAR_EARCUT
