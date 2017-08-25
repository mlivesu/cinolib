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
#ifndef CINO_GLCANVAS_H
#define CINO_GLCANVAS_H

#ifdef CINOLIB_USES_OPENGL
#ifdef CINOLIB_USES_QGLVIEWER

#include <QGLWidget>
#include <vector>
#include <string>

#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

#include <cinolib/cinolib.h>
#include <cinolib/bbox.h>
#include <cinolib/drawable_object.h>

#include <qglviewer.h>

namespace cinolib
{

class GLcanvas : public QGLViewer
{
    public:

        GLcanvas(QWidget * parent = 0);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void init();
        void draw();
        void clear();
        void set_clear_color(const QColor & color);
        void fit_scene();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void push_obj(DrawableObject * obj, bool refit_scene = true);
        bool pop_first_occurrence_of(int type);
        bool pop_all_occurrences_of (int type);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void set_slice(float thresh, int item, int sign, int mode);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::string serialize_drawlist() const;
        std::string serialize_camera() const;
        void        deserialize_camera(const std::string & s);

    private:

        QColor clear_color;
        std::vector<DrawableObject*> drawlist;
};

}

#ifndef  CINO_STATIC_LIB
#include "glcanvas.cpp"
#endif

#endif // #ifdef CINOLIB_USES_QGLVIEWER
#endif // #ifdef CINOLIB_USES_OPENGL

#endif // CINO_GLCANVAS_H
