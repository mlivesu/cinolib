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
#ifndef CINO_HEX_TRANSITION_H
#define CINO_HEX_TRANSITION_H

#include <cinolib/meshes/meshes.h>

namespace cinolib
{

/* This code instantiates an all hex transition scheme to transform an adaptively
 * refined grid into a pure hexahedral mesh. Schemes can be positioned as needed,
 * specifying a center, scale and orientation.
 *
 * For more info on the schemes, and their default size, positioning and orientation
 * refer to cinolib/hex_transition_schemes.h
*/

enum class HexTransition
{
    FLAT_FACE_4_TO_2_0,      // transition from 4x4 to 2x2 grid: flat region
    FLAT_FACE_4_TO_2_1,      // transition from 4x4 to 2x2 grid: flat region (1 convex edge  incident on top)
    FLAT_FACE_4_TO_2_2_diag, // transition from 4x4 to 2x2 grid: flat region (2 convex edges incident on top)
    FLAT_FACE_4_TO_2_2_line, // transition from 4x4 to 2x2 grid: flat region (2 convex edges incident on top)
    FLAT_FACE_4_TO_2_3,      // transition from 4x4 to 2x2 grid: flat region (3 convex edges incident on top)
    FLAT_FACE_4_TO_2_4,      // transition from 4x4 to 2x2 grid: flat region (4 convex edges incident on top)
    CONV_EDGE_4_TO_2,        // transition from 4x4 to 2x2 grid: convex  edge
    CONC_EDGE_4_TO_2,        // transition from 4x4 to 2x2 grid: concave edge
    CONC_VERT_4_TO_2,        // transition from 4x4 to 2x2 grid: concave vertex (3 concave incident edges)
    HYBR_VERT_4_TO_2         // transition from 4x4 to 2x2 grid: hybrid  vertex (3 convex + 3 concave incident edges)
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void hex_transition(Polyhedralmesh<M,V,E,F,P> & m,
                    const HexTransition         type,
                    const vec3d               & center      = vec3d(0,0,0),
                    const double                scale       = 1.0,
                    const int                   orientation = PLUS_Y);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void hex_transition(const HexTransition                    type,
                          std::vector<double>            & verts,
                          std::vector<std::vector<uint>> & faces,
                          std::vector<std::vector<uint>> & polys,
                          std::vector<std::vector<bool>> & winding,
                    const vec3d                          & center,
                    const double                           scale,
                    const int                              orientation);

}

#ifndef  CINO_STATIC_LIB
#include "hex_transition.cpp"
#endif

#endif // CINO_HEX_TRANSITION_H
