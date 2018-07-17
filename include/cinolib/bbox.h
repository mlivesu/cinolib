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
#ifndef CINO_BBOX_H
#define CINO_BBOX_H

#include <cinolib/inf.h>
#include <cinolib/geometry/vec3.h>
#include <algorithm>

namespace cinolib
{

class Bbox
{
    public:

        explicit Bbox() { reset(); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void               reset();
        vec3d              center()    const;
        double             diag()      const;
        double             delta_x()   const;
        double             delta_y()   const;
        double             delta_z()   const;
        vec3d              delta()     const;
        double             min_entry() const;
        double             max_entry() const;
        std::vector<vec3d> corners(const double scaling_factor = 1.0) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d min, max;
};

CINO_INLINE std::ostream & operator<<(std::ostream & in, const Bbox & bb);

}

#ifndef  CINO_STATIC_LIB
#include "bbox.cpp"
#endif

#endif // CINO_BBOX_H
