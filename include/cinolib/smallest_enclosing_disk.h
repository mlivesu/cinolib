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
#ifndef CINO_SMALLEST_ENCLOSING_DISK_H
#define CINO_SMALLEST_ENCLOSING_DISK_H

#include <vector>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec2.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

// Solve the smallest-circle problem (or minimum covering circle problem)
// using a recirsive implementation of the Welzl's algorithm. References:
//
//    1) https://en.wikipedia.org/wiki/Smallest-circle_problem
//
//    2) Smallest Enclosing Disks (Balls and Ellipsoids)
//       Emo Welzl
//       New Results and Trends in Computer Science
//       Lecture Notes in Computer Science, 555 (1991), pp359-370
//
CINO_INLINE
void smallest_enclosing_disk(const std::vector<vec2d> & points,
                                   vec2d              & center,
                                   double             & radius);


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void smallest_enclosing_disk(const std::vector<vec3d> & points,  // will drop z component
                                   vec3d              & center,  // will have z=0
                                   double             & radius);
}

#ifndef  CINO_STATIC_LIB
#include "smallest_enclosing_disk.cpp"
#endif

#endif // CINO_SMALLEST_ENCLOSING_DISK_H
