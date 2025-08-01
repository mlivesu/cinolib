/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2019: Tommaso Sorgente
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
*     Tommaso Sorgente (tommaso.sorgente@.cnr.it)                         *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/

#ifndef CINO_READ_RF_H
#define CINO_READ_RF_H

#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec_mat.h>
#include <sys/types.h>
#include <vector>

namespace cinolib
{

CINO_INLINE
void read_RF(const char                     * basename,
             std::vector<vec3d>             & verts,
             std::vector<std::vector<uint>> & faces,
             std::vector<std::vector<uint>> & polys,
             std::vector<std::vector<bool>> & polys_winding);

}

#ifndef CINO_STATIC_LIB
#include "read_RF.cpp"
#endif

#endif // CINO_READ_RF_H
