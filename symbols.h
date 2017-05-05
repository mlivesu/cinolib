/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2017                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#ifndef CINO_SYMBOLS_H
#define CINO_SYMBOLS_H

namespace cinolib
{

// List of GLOBAL symbols used throughout the library. This helps keeping a unique ID for each symbol.
// Symbols that require specialized values (e.g. X=>0, Y=>1, Z=>2) are to be defined within the classes
// that need them, and will therefore leave in a different namespace.

enum
{
    EQ,   // ==
    NEQ,  // !=
    LEQ,  // <=
    GEQ,  // >=
    AND,  // &&
    OR    // ||
};

}


#endif // CINO_SYMBOLS_H
