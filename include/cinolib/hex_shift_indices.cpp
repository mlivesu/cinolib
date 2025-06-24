/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2025: Marco Livesu                                              *
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
#include <cinolib/hex_shift_indices.h>
#include <stdexcept>

namespace cinolib
{

// Reorders the list of 8 vertices describing a hexahedron H such that the vertex
// positioned at H[offset] moves to H[0]. This is often useful to simplify code for
// hex subdivision
//
CINO_INLINE
void hex_shift_indices(const uint hex_in[8],
                       const uint offset,
                             uint hex_out[8])
{
    // see Table 4 in "How to Subdivide Pyramids, Prisms and Hexahedra into Tetrahedra"
    switch(offset)
    {
        case 0: hex_out[0] = hex_in[0];
                hex_out[1] = hex_in[1];
                hex_out[2] = hex_in[2];
                hex_out[3] = hex_in[3];
                hex_out[4] = hex_in[4];
                hex_out[5] = hex_in[5];
                hex_out[6] = hex_in[6];
                hex_out[7] = hex_in[7];
                return;

        case 1: hex_out[0] = hex_in[1];
                hex_out[1] = hex_in[0];
                hex_out[2] = hex_in[4];
                hex_out[3] = hex_in[5];
                hex_out[4] = hex_in[2];
                hex_out[5] = hex_in[3];
                hex_out[6] = hex_in[7];
                hex_out[7] = hex_in[6];
                return;

        case 2: hex_out[0] = hex_in[2];
                hex_out[1] = hex_in[1];
                hex_out[2] = hex_in[5];
                hex_out[3] = hex_in[6];
                hex_out[4] = hex_in[3];
                hex_out[5] = hex_in[0];
                hex_out[6] = hex_in[4];
                hex_out[7] = hex_in[7];
                return;

        case 3: hex_out[0] = hex_in[3];
                hex_out[1] = hex_in[0];
                hex_out[2] = hex_in[1];
                hex_out[3] = hex_in[2];
                hex_out[4] = hex_in[7];
                hex_out[5] = hex_in[4];
                hex_out[6] = hex_in[5];
                hex_out[7] = hex_in[6];
                return;

        case 4: hex_out[0] = hex_in[4];
                hex_out[1] = hex_in[0];
                hex_out[2] = hex_in[3];
                hex_out[3] = hex_in[7];
                hex_out[4] = hex_in[5];
                hex_out[5] = hex_in[1];
                hex_out[6] = hex_in[2];
                hex_out[7] = hex_in[6];
                return;

        case 5: hex_out[0] = hex_in[5];
                hex_out[1] = hex_in[1];
                hex_out[2] = hex_in[0];
                hex_out[3] = hex_in[4];
                hex_out[4] = hex_in[6];
                hex_out[5] = hex_in[2];
                hex_out[6] = hex_in[3];
                hex_out[7] = hex_in[7];
                return;

        case 6: hex_out[0] = hex_in[6];
                hex_out[1] = hex_in[2];
                hex_out[2] = hex_in[1];
                hex_out[3] = hex_in[5];
                hex_out[4] = hex_in[7];
                hex_out[5] = hex_in[3];
                hex_out[6] = hex_in[0];
                hex_out[7] = hex_in[4];
                return;

        case 7: hex_out[0] = hex_in[7];
                hex_out[1] = hex_in[3];
                hex_out[2] = hex_in[2];
                hex_out[3] = hex_in[6];
                hex_out[4] = hex_in[4];
                hex_out[5] = hex_in[0];
                hex_out[6] = hex_in[1];
                hex_out[7] = hex_in[5];
                return;

        default: throw std::runtime_error("Invalid Hex Offset");
    }
}

}
