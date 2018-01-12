/*********************************************************************************
*  Copyright(C) 2016: Marco Livesu                                               *
*  All rights reserved.                                                          *
*                                                                                *
*  This file is part of CinoLib                                                  *
*                                                                                *
*  CinoLib is dual-licensed:                                                     *
*                                                                                *
*   - For non-commercial use you can redistribute it and/or modify it under the  *
*     terms of the GNU General Public License as published by the Free Software  *
*     Foundation; either version 3 of the License, or (at your option) any later *
*     version.                                                                   *
*                                                                                *
*   - If you wish to use it as part of a commercial software, a proper agreement *
*     with the Author(s) must be reached, based on a proper licensing contract.  *
*                                                                                *
*  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE       *
*  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.     *
*                                                                                *
*  Author(s):                                                                    *
*                                                                                *
*     Marco Livesu (marco.livesu@gmail.com)                                      *
*     http://pers.ge.imati.cnr.it/livesu/                                        *
*                                                                                *
*     Italian National Research Council (CNR)                                    *
*     Institute for Applied Mathematics and Information Technologies (IMATI)     *
*     Via de Marini, 6                                                           *
*     16149 Genoa,                                                               *
*     Italy                                                                      *
**********************************************************************************/
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
