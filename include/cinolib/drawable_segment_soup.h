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
#ifndef CINO_DRAWABLE_SEGMENT_SOUP_H
#define CINO_DRAWABLE_SEGMENT_SOUP_H

#include <vector>
#include <cinolib/geometry/vec3.h>
#include <cinolib/drawable_object.h>
#include <cinolib/color.h>


namespace cinolib
{

/* Utility class to render a soup of segments into a GL canvas.
 * Segment endpoints are stored as a serialized list of vertices,
 * that is:
 *
 * 1st segment: soup[0], soup[1]
 * 2nd segment: soup[2], soup[3]
 * ....
 * nth segment : soup[2(n-1)], soup[2(n-1)+1]
*/

class DrawableSegmentSoup: public std::vector<vec3d>, public DrawableObject
{
    public:

        explicit DrawableSegmentSoup();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void draw(const float scene_size=1) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void set_cheap_rendering(const bool b);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d scene_center() const { return vec3d(); } // TODO
        float scene_radius() const { return 0.0;     }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        ObjectType object_type()  const { return DRAWABLE_CURVE; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void set_color(const Color & c);
        void set_thickness(float t);

    private:

        bool  use_gl_lines; // to speedup rendering (when lots of segments are to be rendered)
        Color color;
        float thickness;
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_segment_soup.cpp"
#endif

#endif // CINO_DRAWABLE_SEGMENT_SOUP_H
