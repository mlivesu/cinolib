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
        bool     render_axis   = false;        // toggle axis rendering
        bool     mouse_pressed = false;        // true if mouse is pressed
        double   radius        = 0.5;          // trackball radius
        vec3d    pivot         = vec3d(0,0,0); // trackball rotation origin
        QPoint   last_click_2d;                // window coords
        vec3d    last_click_3d;                // world  coords
        GLdouble projection[16];               // openGL projection matrix
        GLdouble modelview[16];                // openGL modelview  matrix
        float    z_near        = 0.1;          // front clipping plane
        float    z_far         = 1.0;          // back  clipping plane
        vec3d    scene_center  = vec3d(0,0,0); // scene_center
        float    scene_size    = 1.0;          // scene_size
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

        void keyPressEvent        (QKeyEvent   *event);
        void mouseDoubleClickEvent(QMouseEvent *event);
        void mousePressEvent      (QMouseEvent *event);
        void mouseReleaseEvent    (QMouseEvent *event);
        void mouseMoveEvent       (QMouseEvent *event);
        void wheelEvent           (QWheelEvent *event);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void initializeGL();
        void resizeGL(int w, int h);
        void paintGL();
        void updateGL();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void draw_axis();
        void draw_helper();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void make_popup_menu();
        void set_clear_color(const QColor & c);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void fit_scene();
        void set_scene_center(const vec3d & center);
        void update_projection_matrix(void);
        void map_to_sphere(const QPoint & v2D, vec3d & v3D) const;
        bool unproject(const QPoint & p2d, vec3d & p3d) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void rotate(const vec3d & axis, const double angle);
        void translate(const vec3d & trans);
        void zoom(double d);

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

        QColor    clear_color;
        Trackball trackball;
        QMenu    *popup;
        QFont     font;
        bool      show_helper;

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
