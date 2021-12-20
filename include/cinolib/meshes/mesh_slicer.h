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
#ifndef CINO_MESH_SLICER_H
#define CINO_MESH_SLICER_H

#include <cinolib/symbols.h>
#include <sstream>

namespace cinolib
{

struct SlicerState
{
    float X_thresh  =  1;  // X coord \in [0,1] ( relative w.r.t. bbox delta x)
    float Y_thresh  =  1;  // Y coord \in [0,1] ( relative w.r.t. bbox delta y)
    float Z_thresh  =  1;  // Z coord \in [0,1] ( relative w.r.t. bbox delta z)
    float Q_thresh  =  1;  // Quality \in [0,1] ( assumes quality metric is in [0,1] too)
    int   L_filter  = -1;  // Label (-1 : show all labels)
    bool  X_leq    = true; // { LEQ, GEQ   }
    bool  Y_leq    = true; // { LEQ, GEQ   }
    bool  Z_leq    = true; // { LEQ, GEQ   }
    bool  Q_leq    = true; // { LEQ, GEQ   }
    bool  L_is     = true; // { IS, IS_NOT }
    bool  mode_AND = true; // { AND, OR    }

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    void reset()
    {
        X_thresh =  1;
        Y_thresh =  1;
        Z_thresh =  1;
        Q_thresh =  1;
        L_filter = -1;
        X_leq    = true;
        Y_leq    = true;
        Z_leq    = true;
        Q_leq    = true;
        L_is     = true;
        mode_AND = true;
    }

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    void from_string(const std::string & str)
    {
        std::stringstream ss(str);
        ss >> X_thresh >> Y_thresh >> Z_thresh >> Q_thresh >> L_filter
           >> X_leq >> Y_leq >> Z_leq >> Q_leq >> L_is >> mode_AND;
    }

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    std::string to_string() const
    {
        std::stringstream ss;
        ss << X_thresh << " "
           << Y_thresh << " "
           << Z_thresh << " "
           << Q_thresh << " "
           << L_filter << " "
           << X_leq    << " "
           << Y_leq    << " "
           << Z_leq    << " "
           << Q_leq    << " "
           << L_is     << " "
           << mode_AND;
        return ss.str();
    }
};

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
