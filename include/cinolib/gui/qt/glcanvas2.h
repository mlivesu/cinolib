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
#ifndef CINO_GLCANVAS2_H
#define CINO_GLCANVAS2_H

#ifdef CINOLIB_USES_OPENGL
#ifdef CINOLIB_USES_QT

#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

#include <vector>
#include <string>
#include <QOpenGLWidget>
#include <QColor>
#include <QMenu>
#include <cinolib/drawable_object.h>
#include <cinolib/bbox.h>


namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    double   radius        = 0.5;
    double   radius_sqrd   = radius*radius;
    bool     mouse_pressed = false;
    QPoint   last_point_2d;
    vec3d    last_point_3d;
    GLdouble projection_matrix[16];
    GLdouble modelview_matrix[16];
}
Trackball;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

class GLcanvas2 : public QOpenGLWidget
{
    public:

        explicit GLcanvas2(QWidget * parent = 0);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void mousePressEvent  (QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void mouseMoveEvent   (QMouseEvent *event);
        void wheelEvent       (QWheelEvent *event);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void initializeGL();
        void resizeGL(int w, int h);
        void paintGL();
        void updateGL();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void fit_scene();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_projection_matrix(void);
        void map_to_sphere(const QPoint & v2D, vec3d & v3D);
        void translate(const vec3d & trans);
        void rotate(const vec3d & axis, const double angle);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void push_obj(const DrawableObject * obj, bool refit_scene = true);
        bool pop(const DrawableObject * obj);
        bool pop_first_occurrence_of(int type);
        bool pop_all_occurrences_of (int type);

    private:

        vec3d     scene_center;
        float     scene_radius;
        Trackball trackball;

        std::vector<const DrawableObject*> drawlist;
};

}

#ifndef  CINO_STATIC_LIB
#include "glcanvas2.cpp"
#endif

#endif // #ifdef CINOLIB_USES_QT_AND_QGLVIEWER
#endif // #ifdef CINOLIB_USES_OPENGL

#endif // CINO_GLCANVAS2_H
