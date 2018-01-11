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
#ifndef CINO_MAP_DISTORTION_H
#define CINO_MAP_DISTORTION_H

#include <cinolib/cino_inline.h>

namespace cinolib
{

/* Compute the aspect ratio distortion of a map. Let
 * T be the affine map between A and B, such that:
 *
 *          B = T(A) = M * A + t
 *
 * with M being a 3x3 matrix encoding the rotation/scaling
 * part of the mapping and t be a translation. The aspect
 * ratio distortion induced by T is defined as the ratio
 * between the highest and the lowest eigen values of M,
 * that is:
 *
 *          dist = simga_1(M) / |sigma_3(M)|
 *
 * Ref:
 * Injective and Bounded Distortion Mappings in 3D
 * N.Aigerman and Y.Lipman
 * ACM Transactions on Graphics - SIGGRAPH - 2013
*/
CINO_INLINE
double aspect_ratio_distortion(const double m[3][3]);

}


#ifndef  CINO_STATIC_LIB
#include "map_distortion.cpp"
#endif

#endif // CINO_MAP_DISTORTION_H
