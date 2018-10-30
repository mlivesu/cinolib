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
#ifndef CINO_SYMBOLS_H
#define CINO_SYMBOLS_H

namespace cinolib
{

// List of GLOBAL symbols used throughout the library. This helps keeping a unique ID for each symbol.
// Symbols that require specialized values (e.g. when used for array lookup) are to be defined locally
// (within the files that need them), and to avoid conflicts should live in a dedicated namespace

enum
{
    // XYZ coordinates
    X,
    Y,
    Z,

    // UVW parametric coordinates (and all possible pairings of them - for serialization)
    U_param,
    V_param,
    W_param,
    UV_param,
    UW_param,
    VW_param,
    UVW_param,

    // Quality, Label (for slicer)
    Q,
    L,

    // math/logical binary operators
    EQ,     // ==
    NEQ,    // !=
    LEQ,    // <=
    GEQ,    // >=
    AND,    // &&
    OR,     // ||

    // angle units
    DEG,
    RAD,

    // laplacians
    COTANGENT,
    UNIFORM,

    IS,
    IS_NOT,
};

}


#endif // CINO_SYMBOLS_H
