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
#include <cinolib/drawable_object.h>
#include <cinolib/bbox.h>

namespace cinolib
{

class GLcanvas : public QGLWidget
{
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    typedef struct
    {
        bool     render_axis   = false;
        bool     mouse_pressed = false;
        double   r             = 0.5;
        double   r_sqrd        = r*r;
        QPoint   last_point_2d;
        vec3d    last_point_3d;
        GLdouble projection[16];
        GLdouble modelview[16];
    }
    Trackball;

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    typedef struct
    {
        std::string label;
        vec3d       xyz;   // pos 3D
        uint        x,y;   // pos 2D
        bool        is_3d; // if true the label will be positioned projecting xyz on screen,
                           // otherwise (x,y) will be used as window coordiantes to position it
    }
    TextLabel;

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    public:

        explicit GLcanvas(QWidget * parent = 0);
        ~GLcanvas();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void keyPressEvent    (QKeyEvent   *event);
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

        void make_popup_menu();
        void set_clear_color(const QColor & c);
        void draw_axis();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void fit_scene();
        void update_projection_matrix(void);
        void map_to_sphere(const QPoint & v2D, vec3d & v3D) const;
        void rotate(const vec3d & axis, const double angle);
        void translate(const vec3d & trans);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void push_obj(const DrawableObject * obj, bool refit_scene = true);
        bool pop(const DrawableObject * obj);
        bool pop_first_occurrence_of(int type);
        bool pop_all_occurrences_of (int type);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void push_label(const vec3d & p, const std::string & label);
        void push_label(const uint x, const uint y, const std::string & label);
        void pop_label();
        void pop_all_labels();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::string serialize_POV() const;
        void        deserialize_POV(const std::string & s);

    private:

        vec3d     scene_center;
        float     scene_radius;
        QColor    clear_color;
        float     z_near_plane; // view frustum front clipping plane
        float     z_far_plane;  // view frustum back  clipping plane
        Trackball trackball;
        QMenu    *popup;

        std::vector<const DrawableObject*> objects;
        std::vector<const TextLabel>       labels;
};

}

#ifndef  CINO_STATIC_LIB
#include "glcanvas.cpp"
#endif

#endif // #ifdef CINOLIB_USES_QT
#endif // #ifdef CINOLIB_USES_OPENGL

#endif // CINO_GLCANVAS_H
