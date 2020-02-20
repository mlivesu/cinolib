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

        ~DrawableSegmentSoup(){}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        ObjectType object_type() const { return DRAWABLE_CURVE; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void draw(const float scene_size=1) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d scene_center() const { return vec3d(); } // TODO
        float scene_radius() const { return 0.0;     }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void push_seg(const vec3d v0, const vec3d v1);
        void pop_seg();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void set_color          (const Color & c);
        void set_thickness      (float t);
        void set_cheap_rendering(const bool b);
        void set_always_in_front(const bool b);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    private:

        bool  no_depth_test; // render segments always in front, regardless of what's in the GL scene
        bool  use_gl_lines;  // to speedup rendering (when lots of segments are to be rendered)
        Color color;
        float thickness;
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_segment_soup.cpp"
#endif

#endif // CINO_DRAWABLE_SEGMENT_SOUP_H
