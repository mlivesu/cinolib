/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
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
#ifndef TRIANGLE_WRAP_H
#define TRIANGLE_WRAP_H

#include <cinolib/cinolib.h>

#include <vector>
#include <sys/types.h>

namespace cinolib
{

CINO_INLINE
void triangle_wrap(const std::vector<double> & coords_in,   // serialized xy coordinates
                   const std::vector<uint>   & segs_in,     // serialized segment endpoint ids
                   const std::vector<double> & holes_in,    // serialized xy holes innerpoints
                   const double                z_coord,     // z coord for coords_out
                   const std::string         & flags,       // options
                         std::vector<double> & coords_out,  // serialized xyz's (z = z_coord)
                         std::vector<uint>   & tris_out);   // serialized triangle vertex ids

}

#ifndef  CINO_STATIC_LIB
#include <cinolib/triangle_wrap.cpp>
#endif

#endif // TRIANGLE_WRAP_H
