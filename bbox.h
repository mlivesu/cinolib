/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#ifndef BBOX_H
#define BBOX_H

#include <float.h>
#include "vec3.h"
#include "cino_inline.h"

namespace cinolib
{

class Bbox
{
    public:

        Bbox() { reset(); }

        void reset()
        {
            min = vec3d( FLT_MAX,  FLT_MAX,  FLT_MAX);
            max = vec3d(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        }

        vec3d  min, max;
        inline vec3d  center()    const { return (min + max) * 0.5;    }
        inline double diag()      const { return (min - max).length(); }
        inline double delta_x()   const { return (max.x() - min.x());  }
        inline double delta_y()   const { return (max.y() - min.y());  }
        inline double delta_z()   const { return (max.z() - min.z());  }
        inline vec3d  delta()     const { return (max     - min);      }
        inline double min_entry() const { return  std::min(min.min_entry(), max.min_entry()); }
        inline double max_entry() const { return  std::max(max.max_entry(), max.max_entry()); }
};

CINO_INLINE std::ostream & operator<<(std::ostream & in, const Bbox & bb);

}

#ifndef  CINO_STATIC_LIB
#include "bbox.cpp"
#endif

#endif // BBOX_H
