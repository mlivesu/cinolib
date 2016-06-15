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
#ifndef GLCANVAS_H
#define GLCANVAS_H

#include <qglviewer.h>
#include <QGLWidget>
#include <vector>

#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

#include "../cino_inline.h"
#include "../bbox.h"
#include "../drawable_object.h"


namespace cinolib
{

class GLcanvas : public QGLViewer
{
    public:

        GLcanvas(QWidget * parent = 0);

        void init();
        void draw();
        void clear();
        void fit_scene();
        void set_clear_color(const QColor & color);

        void push_obj(const DrawableObject * obj, bool refit_scene = true);

        bool pop_first_occurrence_of(int type);
        bool pop_all_occurrences_of (int type);

    private:

        QColor clear_color;
        std::vector<const DrawableObject *> drawlist;
};


}


#ifndef  CINO_STATIC_LIB
#include "glcanvas.cpp"
#endif

#endif // GLCANVAS_H
