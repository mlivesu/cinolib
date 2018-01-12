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

        void reset()
        {
            min = vec3d( inf_double,  inf_double,  inf_double);
            max = vec3d(-inf_double, -inf_double, -inf_double);
        }

        vec3d  min, max;
        inline vec3d  center()    const { return (min + max) * 0.5;    }
        inline double diag()      const { return (min - max).length(); }
        inline double delta_x()   const { return (max.x() - min.x());  }
        inline double delta_y()   const { return (max.y() - min.y());  }
        inline double delta_z()   const { return (max.z() - min.z());  }
        inline vec3d  delta()     const { return (max     - min);      }
        inline double min_entry() const { return  std::min(min.min_entry(), max.min_entry()); }
        inline double max_entry() const { return  std::max(min.max_entry(), max.max_entry()); }
};

CINO_INLINE std::ostream & operator<<(std::ostream & in, const Bbox & bb);

}

#ifndef  CINO_STATIC_LIB
#include "bbox.cpp"
#endif

#endif // CINO_BBOX_H
