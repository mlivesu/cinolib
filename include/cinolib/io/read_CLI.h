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
#ifndef CINO_READ_CLI_H
#define CINO_READ_CLI_H

#include <vector>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Reference for COMMON LAYER INTERFACE (CLI) file format:
// http://www.hmilch.net/downloads/cli_format.html
//
// NOTE: output vectors have as many entries as the number of slices. Each
// slice contains a list of internal, external and open polylines. Finally,
// each polyline is a vector of points in 3D.
//
CINO_INLINE
void read_CLI(const char                                   * filename,
              std::vector<std::vector<std::vector<vec3d>>> & internal_polylines, // outer slice boundary
              std::vector<std::vector<std::vector<vec3d>>> & external_polylines, // inner holes
              std::vector<std::vector<std::vector<vec3d>>> & open_polylines,     // support structures
              std::vector<std::vector<std::vector<vec3d>>> & hatches);           // supports/infills
}

#ifndef  CINO_STATIC_LIB
#include "read_CLI.cpp"
#endif

#endif // CINO_READ_CLI_H
