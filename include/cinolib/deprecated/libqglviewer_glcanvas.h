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
#include <cinolib/geometry/aabb.h>


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
