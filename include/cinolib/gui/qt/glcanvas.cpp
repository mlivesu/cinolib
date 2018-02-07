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
#include <cinolib/gui/qt/glcanvas.h>
#include <cinolib/cino_inline.h>
#include <cinolib/gl/draw_sphere.h>
#include <cinolib/gl/draw_cylinder.h>
#include <cinolib/color.h>

#include <sstream>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QGLWidget>
#include <QShortcut>
#include <QMouseEvent>
#include <QMimeData>
#include <QMenu>

namespace cinolib
{

CINO_INLINE
GLcanvas::GLcanvas(QWidget *parent) : QOpenGLWidget(parent), QOpenGLFunctions()
{
    setFocusPolicy(Qt::StrongFocus);

    scene_radius = 1.0;
    scene_center = vec3d(0,0,0);
    z_near_plane = scene_radius*0.5;
    z_far_plane  = scene_radius*10.0;
    clear_color  = QColor(200, 200, 200);

    make_popup_menu();

    // enable cut/paste shortcuts to copy/paste points of view for fast reproduction of paper images/comparisons
    //
    connect(new QShortcut(QKeySequence::Copy, this), &QShortcut::activated, [&](){
        QApplication::clipboard()->setText(serialize_POV().data());
    });
    //
    connect(new QShortcut(QKeySequence::Paste, this), &QShortcut::activated, [&](){
        if (QApplication::clipboard()->mimeData()->hasText())
        {
            deserialize_POV(QApplication::clipboard()->mimeData()->text().toStdString());
        }
    });
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
GLcanvas::~GLcanvas()
{
    delete popup;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::initializeGL()
{
    initializeOpenGLFunctions();

    makeCurrent();
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    // scene pos and size
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetDoublev(GL_MODELVIEW_MATRIX, trackball.modelview);
    update_projection_matrix();
    fit_scene();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::resizeGL(int w, int h)
{
    makeCurrent();
    update_projection_matrix();
    glViewport(0, 0, w, h);
    update();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::paintGL()
{
    glClearColor(clear_color.redF(), clear_color.greenF(), clear_color.blueF(), clear_color.alphaF());
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    for(auto obj : drawlist) obj->draw(scene_radius);

    if (trackball.render_axis) draw_axis();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::fit_scene()
{
    scene_center = vec3d(0,0,0);
    scene_radius = 0.0;

    int count  = 0;
    for(const DrawableObject *obj : drawlist)
    {
        scene_center += obj->scene_center();
        scene_radius  = std::max(scene_radius, obj->scene_radius());
        ++count;
    }
    if (count>0) scene_center /= (double)count;

    z_near_plane =  0.5 * scene_radius;
    z_far_plane  = 10.0 * scene_radius;

    translate(vec3d(-(trackball.modelview[0]*scene_center[0] +
                      trackball.modelview[4]*scene_center[1] +
                      trackball.modelview[8]*scene_center[2] +
                      trackball.modelview[12]),
                    -(trackball.modelview[1]*scene_center[0] +
                      trackball.modelview[5]*scene_center[1] +
                      trackball.modelview[9]*scene_center[2] +
                      trackball.modelview[13]),
                    -(trackball.modelview[2]*scene_center[0] +
                      trackball.modelview[6]*scene_center[1] +
                      trackball.modelview[10]*scene_center[2] +
                      trackball.modelview[14] +
                      3.0*scene_radius)));

    update_projection_matrix();
    updateGL();

    //std::cout << std::endl;
    //std::cout << "Scene Center: " << scene_center << std::endl;
    //std::cout << "Scene Radius: " << scene_radius << std::endl;
    //std::cout << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::set_clear_color(const QColor &c)
{
    clear_color = c;
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::draw_axis()
{
    vec3d O = scene_center;
    vec3d X = scene_center + vec3d(1,0,0)*scene_radius;
    vec3d Y = scene_center + vec3d(0,1,0)*scene_radius;
    vec3d Z = scene_center + vec3d(0,0,1)*scene_radius;

    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_DEPTH_TEST);
    cylinder(O, X, scene_radius*0.015, scene_radius*0.015, Color::RED().rgba);
    cylinder(O, Y, scene_radius*0.015, scene_radius*0.015, Color::GREEN().rgba);
    cylinder(O, Z, scene_radius*0.015, scene_radius*0.015, Color::BLUE().rgba);
    sphere(O, scene_radius*0.02, Color::WHITE().rgba);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::updateGL()
{
    update();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::update_projection_matrix()
{
    makeCurrent();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)width()/(GLfloat)height(), z_near_plane, z_far_plane);
    glGetDoublev(GL_PROJECTION_MATRIX, trackball.projection);
    glMatrixMode(GL_MODELVIEW);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Arcball rotation control
// Ken Shoemake
// Graphics Gems IV, 1993
//
CINO_INLINE
void GLcanvas::map_to_sphere(const QPoint & p2d, vec3d & p3d) const
{
    double x =  (2.0*p2d.x() - width())  / width();
    double y = -(2.0*p2d.y() - height()) / height();
    double xval = x;
    double yval = y;
    double x2y2 = xval*xval + yval*yval;

    p3d[0] = xval;
    p3d[1] = yval;
    p3d[2] = (x2y2 < 0.5*trackball.r) ? sqrt(trackball.r - x2y2) : 0.5*trackball.r/sqrt(x2y2);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::translate(const vec3d & t)
{
    makeCurrent();
    glLoadIdentity();
    glTranslated(t[0], t[1], t[2]);
    glMultMatrixd(trackball.modelview);
    glGetDoublev(GL_MODELVIEW_MATRIX, trackball.modelview);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::rotate(const vec3d & axis, const double angle)
{
    vec3d t(trackball.modelview[0]*scene_center[0] +
            trackball.modelview[4]*scene_center[1] +
            trackball.modelview[8]*scene_center[2] +
            trackball.modelview[12],
            trackball.modelview[1]*scene_center[0] +
            trackball.modelview[5]*scene_center[1] +
            trackball.modelview[9]*scene_center[2] +
            trackball.modelview[13],
            trackball.modelview[2]*scene_center[0] +
            trackball.modelview[6]*scene_center[1] +
            trackball.modelview[10]*scene_center[2] +
            trackball.modelview[14]);

    makeCurrent();
    glLoadIdentity();
    glTranslatef(t[0], t[1], t[2]);
    glRotated(angle, axis[0], axis[1], axis[2]);
    glTranslatef(-t[0], -t[1], -t[2]);
    glMultMatrixd(trackball.modelview);
    glGetDoublev(GL_MODELVIEW_MATRIX, trackball.modelview);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::push_obj(const DrawableObject *obj, bool refit_scene)
{
    if (obj==NULL) return;
    drawlist.push_back(obj);
    if (refit_scene) fit_scene();
    update();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool GLcanvas::pop_all_occurrences_of(int type)
{
    bool found = false;
    while (pop_first_occurrence_of(type))
    {
        found = true;
    };
    return found;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool GLcanvas::pop_first_occurrence_of(int type)
{
    for(std::vector<const DrawableObject*>::iterator it=drawlist.begin(); it!=drawlist.end(); ++it)
    {
        const DrawableObject *obj = *it;

        if (obj->object_type() == type)
        {
            drawlist.erase(it);
            return true;
        }
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool GLcanvas::pop(const DrawableObject *obj)
{
    for(std::vector<const DrawableObject*>::iterator it=drawlist.begin(); it!=drawlist.end(); ++it)
    {
        if (obj == *it)
        {
            drawlist.erase(it);
            return true;
        }
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_A:     trackball.render_axis=!trackball.render_axis; break;
        case Qt::Key_Left:  rotate(vec3d(0,1,0), -3); break;
        case Qt::Key_Right: rotate(vec3d(0,1,0), +3); break;
        case Qt::Key_Up:    rotate(vec3d(1,0,0), -3); break;
        case Qt::Key_Down:  rotate(vec3d(1,0,0), +3); break;
    }
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button()  == Qt::RightButton &&
        event->buttons() == Qt::RightButton)
    {
        event->accept();
        popup->exec(QCursor::pos());
    }
    else
    {
        event->accept();
        trackball.mouse_pressed = true;
        trackball.last_point_2d = event->pos();
        map_to_sphere(trackball.last_point_2d, trackball.last_point_3d);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    trackball.mouse_pressed = false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::mouseMoveEvent(QMouseEvent *event)
{
    // translate
    if (event->modifiers() & Qt::ShiftModifier)
    {
        event->accept();
        QPoint p2d    = event->pos();
        float  dx     = p2d.x() - trackball.last_point_2d.x();
        float  dy     = p2d.y() - trackball.last_point_2d.y();
        float  w      = width();
        float  h      = height();
        float  aspect = w/h;
        float  top    = tan(45.f/2.0f*M_PI/180.0f) * z_near_plane;
        float  right  = aspect*top;
        float  z      = - (trackball.modelview[ 2]*scene_center[0] +
                           trackball.modelview[ 6]*scene_center[1] +
                           trackball.modelview[10]*scene_center[2] +
                           trackball.modelview[14]) /
                          (trackball.modelview[ 3]*scene_center[0] +
                           trackball.modelview[ 7]*scene_center[1] +
                           trackball.modelview[11]*scene_center[2] +
                           trackball.modelview[15]);
        translate(vec3d(2.0*dx/w*right/z_near_plane*z, -2.0*dy/h*top/z_near_plane*z, 0.0f));
        trackball.last_point_2d = p2d;
        update();
        return;
    }

    // rotate
    if (event->buttons() == Qt::LeftButton)
    {
        event->accept();
        QPoint p2d = event->pos();
        vec3d  p3d;
        map_to_sphere(p2d, p3d);
        if (trackball.mouse_pressed)
        {
            vec3d axis = trackball.last_point_3d.cross(p3d);
            if (axis.length_squared()<1e-7) axis = vec3d(1,0,0);
            axis.normalize();
            vec3d d = trackball.last_point_3d - p3d;
            float t = 0.5*d.length()/trackball.r;
            if (t < -1.0) t = -1.0;
            else if ( t > 1.0 ) t = 1.0;
            float   phi = 2.0*asin(t);
            float angle = phi*180.0/M_PI;
            rotate(axis, angle);
        }
        trackball.last_point_2d = p2d;
        trackball.last_point_3d = p3d;
        update();
        return;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::wheelEvent(QWheelEvent *event)
{
    event->accept();
    float d = -(float) event->delta()/120.0*0.2*scene_radius;
    translate(vec3d(0,0,d));
    // Jan 27, 2018: maybe move clipping planes to avoid front clipping?
    update();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::string GLcanvas::serialize_POV() const
{
    std::stringstream ss;
    ss << trackball.modelview[ 0] << " " << trackball.modelview[ 1] << " " << trackball.modelview[ 2] << " " << trackball.modelview[ 3] << " "
       << trackball.modelview[ 4] << " " << trackball.modelview[ 5] << " " << trackball.modelview[ 6] << " " << trackball.modelview[ 7] << " "
       << trackball.modelview[ 8] << " " << trackball.modelview[ 9] << " " << trackball.modelview[10] << " " << trackball.modelview[11] << " "
       << trackball.modelview[12] << " " << trackball.modelview[13] << " " << trackball.modelview[14] << " " << trackball.modelview[15];
    return ss.str();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::deserialize_POV(const std::string & s)
{
    std::stringstream ss(s);
    ss >> trackball.modelview[ 0] >> trackball.modelview[ 1] >> trackball.modelview[ 2] >> trackball.modelview[ 3]
       >> trackball.modelview[ 4] >> trackball.modelview[ 5] >> trackball.modelview[ 6] >> trackball.modelview[ 7]
       >> trackball.modelview[ 8] >> trackball.modelview[ 9] >> trackball.modelview[10] >> trackball.modelview[11]
       >> trackball.modelview[12] >> trackball.modelview[13] >> trackball.modelview[14] >> trackball.modelview[15];
    makeCurrent();
    glLoadIdentity();
    glMultMatrixd(trackball.modelview);
    glGetDoublev(GL_MODELVIEW_MATRIX, trackball.modelview);
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::make_popup_menu()
{
    popup = new QMenu(this);

    QAction *background_color = new QAction("Brackground color", this);
    connect(background_color, &QAction::triggered, [&]() {
        set_clear_color(QColorDialog::getColor(Qt::white, this));
        updateGL();
    });
    popup->addAction(background_color);

    QAction *copy_POV = new QAction("Copy POV", this);
    connect(copy_POV, &QAction::triggered, [&]() {
        QApplication::clipboard()->setText(serialize_POV().data());
    });
    popup->addAction(copy_POV);

    QAction *paste_POV = new QAction("Paste POV", this);
    connect(paste_POV, &QAction::triggered, [&]() {
        if (QApplication::clipboard()->mimeData()->hasText())
        {
            deserialize_POV(QApplication::clipboard()->mimeData()->text().toStdString());
        }
    });
    popup->addAction(paste_POV);
}

}
