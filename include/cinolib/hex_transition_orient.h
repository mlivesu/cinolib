/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2021: Marco Livesu                                              *
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
*     Luca Pitzalis (lucapizza@gmail.com)                                       *
*     University of Cagliari                                                    *
*                                                                               *
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
#ifndef CINO_HEX_TRANSITION_ORIENT_H
#define CINO_HEX_TRANSITION_ORIENT_H

#include <cinolib/geometry/vec_mat.h>
#include <map>

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
    // schemes for strongly balanced grids
    FLAT,
    FLAT_CONVEX,
    CONVEX_1,
    CONVEX_2,
    CONVEX_3,
    EDGE,
    VERT_CENTER,
    VERT_SIDE,

    // additional schemes for weakly balanced grids
    EDGE_WB,
    VERT_CENTER_WB_1,
    VERT_CENTER_WB_2,
    VERT_CENTER_WB_3,
    VERT_SIDE_WB
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

enum class SchemeType
{
    FLAT_S,
    CONC_S,
    CORN_S,
    CONV_S
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

struct SchemeInfo
{
    HexTransition       type;
    SchemeType          scheme_type;
    double              scale;
    std::vector<vec3d>  t_verts;
    std::map<uint,bool> cuts;
    std::vector<int>    orientations;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/* This function sets a hex transition scheme to the right position, scale and orientation.
 * The parameters are:
 *    - verts, faces, polys, winding  => geometry and connectivity of the scheme specified in info
 *    - info                          => struct containing all the information needed to position the scheme
 *    - poly_centroid                 => centroid of the hexahedron that will be replaced with the transition
 */
CINO_INLINE
void hex_transition_orient(std::vector<vec3d>             & verts,
                           std::vector<std::vector<uint>> & faces,
                           std::vector<std::vector<uint>> & polys,
                           std::vector<std::vector<bool>> & winding,
                           const SchemeInfo               & info,
                           const vec3d                      poly_centroid);
}

#ifndef  CINO_STATIC_LIB
#include "hex_transition_orient.cpp"
#endif

#endif // CINO_HEX_TRANSITION_ORIENT_H
