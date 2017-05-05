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
#ifndef CINO_COLOR_H
#define CINO_COLOR_H

#include <cinolib/cinolib.h>

namespace cinolib
{

class Color
{
    public:

        Color(const float r=1.0,
              const float g=1.0,
              const float b=1.0,
              const float a=1.0)
        : r(r), g(g), b(b), a(a)
        {}

        static Color RED()     { return Color(1,0,0); }
        static Color GREEN()   { return Color(0,1,0); }
        static Color BLUE()    { return Color(0,0,1); }
        static Color YELLOW()  { return Color(1,1,0); }
        static Color MAGENTA() { return Color(1,0,1); }
        static Color CYAN()    { return Color(0,1,1); }
        static Color BLACK()   { return Color(0,0,0); }
        static Color WHITE()   { return Color(1,1,1); }

        union
        {
            struct
            {
                float r, g, b, a;
            };
            float rgba[4];
        };
};

}

#ifndef  CINO_STATIC_LIB
#include "color.cpp"
#endif

#endif // CINO_COLOR_H
