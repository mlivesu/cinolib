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

#include <cinolib/meshes/abstract_mesh.h>

namespace cinolib
{

class MeshSlicer
{
    public:

        MeshSlicer(){}
       ~MeshSlicer(){}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        float X_thresh  =  1;   // X coord \in [0,1] (relative w.r.t. bbox delta x)
        float Y_thresh  =  1;   // Y coord \in [0,1] (relative w.r.t. bbox delta y)
        float Z_thresh  =  1;   // Z coord \in [0,1] (relative w.r.t. bbox delta z)
        float Q_thresh  =  1;   // Quality \in [0,1] (assumes quality metric is in [0,1] too)
        int   L_filter  = -1;   // Label             (-1 : show all labels)
        bool  X_leq     = true; // { <=  or >=     }
        bool  Y_leq     = true; // { <=  or >=     }
        bool  Z_leq     = true; // { <=  or >=     }
        bool  Q_leq     = true; // { <=  or >=     }
        bool  L_is      = true; // { IS  or IS_NOT }
        bool  mode_AND  = true; // { AND or OR     }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void reset();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void        deserialize(const std::string & str);
        std::string serialize() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        template<class M, class V, class E, class P>
        void slice(AbstractMesh<M,V,E,P> & m);
};

}

#ifndef  CINO_STATIC_LIB
#include "mesh_slicer.cpp"
#endif

#endif // CINO_MESH_SLICER_H
