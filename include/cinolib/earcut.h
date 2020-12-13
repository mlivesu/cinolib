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
#ifndef CINO_EARCUT_H
#define CINO_EARCUT_H

#include <sys/types.h>
#include <cinolib/cino_inline.h>

namespace cinolib
{

// Implementation of the earcut algorithm, following the guidelines
// contained in:
//
//      Triangulation by ear clipping
//      Geometric Tools (2008)
//      D.Eberly
//
//      FIST: Fast Industrial-Strength Triangulation of Polygons
//      Algorithmica (2001)
//      M.Held
//
// NOTE: using the templated class point, I can feed this routine with
// 3d points and use just the XY component without performing any cast

// Three alternative schemes are proposed for ear processing
enum class EarSorting
{
    // processes ears in the order they are found.
    // May produce numerous badly shaped triangles
    SEQUENTIAL,

    // processes ears in random order. Tends to produce
    // slightly better triangulations than SEQUENTIAL,
    // with substancially no computational overhead
    RANDOMIZED,

    // sorts ears by increasing inner angle.
    // Produces the best triangulations but
    // is introduces a tiny overhead to handle
    // the priority queue
    PRIORITIZED
};

template<class point>
CINO_INLINE
bool earcut(const std::vector<point> & poly,
                  std::vector<uint>  & tris,
            const EarSorting           sort = EarSorting::SEQUENTIAL);
}

#ifndef  CINO_STATIC_LIB
#include "earcut.cpp"
#endif

#endif // CINO_EARCUT_H
