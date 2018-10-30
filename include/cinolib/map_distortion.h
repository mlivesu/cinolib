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
