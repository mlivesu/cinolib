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
#ifndef CINO_MESH_SLICER_H
#define CINO_MESH_SLICER_H

#include <cinolib/symbols.h>

namespace cinolib
{

typedef struct
{
    float X_thresh =  1;  // X coord \in [0,1] ( relative w.r.t. bbox delta x)
    float Y_thresh =  1;  // Y coord \in [0,1] ( relative w.r.t. bbox delta y)
    float Z_thresh =  1;  // Z coord \in [0,1] ( relative w.r.t. bbox delta z)
    float Q_thresh =  1;  // Quality \in [0,1] ( assumes quality metric is in [0,1] too)
    int   L_filter = -1;  // Label (-1 : show all labels)
    int   X_sign   = LEQ; // { LEQ, GEQ   }
    int   Y_sign   = LEQ; // { LEQ, GEQ   }
    int   Z_sign   = LEQ; // { LEQ, GEQ   }
    int   Q_sign   = LEQ; // { LEQ, GEQ   }
    int   L_mode   = IS;  // { IS, IS_NOT }
    int   mode     = AND; // { AND, OR    }
}
SlicerState;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/* Filter mesh elements according to a number of different criteria.
 * Useful to inspect the interior of volume meshes, or to isolate
 * interesting portions of a complex surface mesh.
*/
template<class Mesh>
class MeshSlicer
{
    public:

        explicit MeshSlicer() {}

        explicit MeshSlicer(Mesh & m);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void reset(Mesh & m);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update(Mesh & m, const SlicerState & s);
};

}

#ifndef  CINO_STATIC_LIB
#include "mesh_slicer.cpp"
#endif

#endif // CINO_MESH_SLICER_H
