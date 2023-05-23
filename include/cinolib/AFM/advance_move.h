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
#ifndef CINO_AFM_ADVANCE_MOVE_H
#define CINO_AFM_ADVANCE_MOVE_H

#include <cinolib/AFM/AFM.h>

namespace cinolib
{

// advance front edge v0-v1, adding triangle pid with a vertex split
CINO_INLINE
bool advance_by_triangle_split(      AFM_data & data,
                               const uint       pid, // w.r.t. m0
                                     uint       v0,  // w.r.t. both m0 and m1
                                     uint       v1,  // w.r.t. both m0 and m1
                               const bool       update_split_point_coords = true); // updates the coordinates of the split point

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// advance the front by adding triangle pid with an edge flip
CINO_INLINE
bool advance_by_edge_flip(AFM_data & data, const uint pid);

}

#ifndef  CINO_STATIC_LIB
#include "advance_move.cpp"
#endif

#endif // CINO_AFM_ADVANCE_MOVE_H
