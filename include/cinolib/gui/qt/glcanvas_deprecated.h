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
#ifndef CINO_LIBQGLVIEWER_CANVAS_H
#define CINO_LIBQGLVIEWER_CANVAS_H

#ifdef CINOLIB_YES_I_KNOW_LIBQGLVIEWER_CANVAS_IS_DEPRECATED
#ifdef CINOLIB_USES_OPENGL
#ifdef CINOLIB_USES_QT

#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

#include <vector>
#include <string>
#include <QGLWidget>
#include <QColor>
#include <QMenu>
#include <qglviewer.h>
#include <cinolib/drawable_object.h>
#include <cinolib/bbox.h>


namespace cinolib
{

class GLcanvas : public QGLViewer
{
    public:

        explicit GLcanvas(QWidget * parent = 0);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void make_popup_menu();
        void mousePressEvent(QMouseEvent *event);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void updateGL();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void init();
        void draw();
        void clear();
        void set_clear_color(const QColor & color);
        void fit_scene();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void push_obj(DrawableObject * obj, bool refit_scene = true);
        bool pop(DrawableObject * obj);
        bool pop_first_occurrence_of(int type);
        bool pop_all_occurrences_of (int type);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::string serialize_drawlist() const;
        std::string serialize_camera() const;
        void        deserialize_camera(const std::string & s);

    private:

        QMenu                       *popup;
        QColor                       clear_color;
        std::vector<DrawableObject*> drawlist;
};

}

#ifndef  CINO_STATIC_LIB
#include "glcanvas.cpp"
#endif

#endif // #ifdef CINOLIB_USES_QT
#endif // #ifdef CINOLIB_USES_OPENGL
#endif // #ifdef CINOLIB_YES_I_KNOW_LIBQGLVIEWER_CANVAS_IS_DEPRECATED

#endif // CINO_LIBQGLVIEWER_CANVAS_H
