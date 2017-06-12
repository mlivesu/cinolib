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
#ifndef CINO_DRAWABLE_ISOCONTOUR_H
#define CINO_DRAWABLE_ISOCONTOUR_H

#include <cinolib/cinolib.h>
#include <cinolib/drawable_object.h>
#include <cinolib/meshes/trimesh/trimesh.h>
#include <cinolib/isocontour.h>

namespace cinolib
{

class DrawableIsocontour : public Isocontour, public DrawableObject
{
    public:

        DrawableIsocontour();
        DrawableIsocontour(Trimesh<> & m_ptr, float iso_value);

        // Implement DrawableObject interface
        //
        void  draw(const float scene_size=1)         const;
        vec3d scene_center() const { return vec3d(); }
        float scene_radius() const { return 0.0;     }
        void  slice(const float, const int, const int, const int) {}

    private:

        Color color_sample;
        Color color_centre;
        Color color_cylind;
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_isocontour.cpp"
#endif

#endif // CINO_DRAWABLE_ISOCONTOUR_H
