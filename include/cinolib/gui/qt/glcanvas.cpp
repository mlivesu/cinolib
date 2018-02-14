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
#include <QShortcut>
#include <QMouseEvent>
#include <QMimeData>
#include <QMenu>

namespace cinolib
{

CINO_INLINE
GLcanvas::GLcanvas(QWidget *parent) : QGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    clear_color = QColor(200, 200, 200);
    make_popup_menu();

    // enable cut/paste shortcuts to copy/paste points of view for fast reproduction of paper images/comparisons
    //
    connect(new QShortcut(QKeySequence::Copy, this), &QShortcut::activated, [&](){
        QApplication::clipboard()->setText(serialize_POV().data());
    });
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
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::paintGL()
{
    makeCurrent();
    glClearColor(clear_color.redF(), clear_color.greenF(), clear_color.blueF(), clear_color.alphaF());
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // render objects
    for(auto obj : objects) obj->draw(trackball.scene_size);

    // render axis
    if (trackball.render_axis) draw_axis();

    // render labels
    for(auto l : labels)
    {
        if (l.is_3d) renderText(l.xyz.x(), l.xyz.y(), l.xyz.z(), l.label.c_str());
        else         renderText(l.x, l.y, l.label.c_str());
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::update_projection_matrix()
{
    makeCurrent();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)width()/(GLfloat)height(), trackball.z_near, trackball.z_far);
    glGetDoublev(GL_PROJECTION_MATRIX, trackball.projection);
    glMatrixMode(GL_MODELVIEW);
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
    vec3d t(trackball.modelview[0]*trackball.scene_center[0] +
            trackball.modelview[4]*trackball.scene_center[1] +
            trackball.modelview[8]*trackball.scene_center[2] +
            trackball.modelview[12],
            trackball.modelview[1]*trackball.scene_center[0] +
            trackball.modelview[5]*trackball.scene_center[1] +
            trackball.modelview[9]*trackball.scene_center[2] +
            trackball.modelview[13],
            trackball.modelview[2]*trackball.scene_center[0] +
            trackball.modelview[6]*trackball.scene_center[1] +
            trackball.modelview[10]*trackball.scene_center[2] +
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
        trackball.last_click_2d = event->pos();
        map_to_sphere(trackball.last_click_2d, trackball.last_click_3d);
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
        float  dx     = p2d.x() - trackball.last_click_2d.x();
        float  dy     = p2d.y() - trackball.last_click_2d.y();
        float  w      = width();
        float  h      = height();
        float  aspect = w/h;
        float  top    = tan(45.f/2.0f*M_PI/180.0f) * trackball.z_near;
        float  right  = aspect*top;
        float  z      = - (trackball.modelview[ 2]*trackball.scene_center[0] +
                           trackball.modelview[ 6]*trackball.scene_center[1] +
                           trackball.modelview[10]*trackball.scene_center[2] +
                           trackball.modelview[14]) /
                          (trackball.modelview[ 3]*trackball.scene_center[0] +
                           trackball.modelview[ 7]*trackball.scene_center[1] +
                           trackball.modelview[11]*trackball.scene_center[2] +
                           trackball.modelview[15]);
        translate(vec3d(2.0*dx/w*right/trackball.z_near*z, -2.0*dy/h*top/trackball.z_near*z, 0.0f));
        trackball.last_click_2d = p2d;
        updateGL();
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
            vec3d axis = trackball.last_click_3d.cross(p3d);
            if (axis.length_squared()<1e-7) axis = vec3d(1,0,0);
            axis.normalize();
            vec3d d = trackball.last_click_3d - p3d;
            float t = 0.5*d.length()/trackball.radius;
            if (t < -1.0) t = -1.0;
            else if ( t > 1.0 ) t = 1.0;
            float   phi = 2.0*asin(t);
            float angle = phi*180.0/M_PI;
            rotate(axis, angle);
        }
        trackball.last_click_2d = p2d;
        trackball.last_click_3d = p3d;
        updateGL();
        return;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
CINO_INLINE
std::string GLcanvas::serialize_POV() const
{
    std::stringstream ss;
    ss << trackball.modelview[ 0] << " " << trackball.modelview[ 1] << " " << trackball.modelview[ 2] << " " << trackball.modelview[ 3] << " "
       << trackball.modelview[ 4] << " " << trackball.modelview[ 5] << " " << trackball.modelview[ 6] << " " << trackball.modelview[ 7] << " "
       << trackball.modelview[ 8] << " " << trackball.modelview[ 9] << " " << trackball.modelview[10] << " " << trackball.modelview[11] << " "
       << trackball.modelview[12] << " " << trackball.modelview[13] << " " << trackball.modelview[14] << " " << trackball.modelview[15] << " "
       << trackball.z_near        << " " << trackball.z_far;
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
       >> trackball.modelview[12] >> trackball.modelview[13] >> trackball.modelview[14] >> trackball.modelview[15]
       >> trackball.z_near        >> trackball.z_far;
    makeCurrent();
    glLoadIdentity();
    glMultMatrixd(trackball.modelview);
    glGetDoublev(GL_MODELVIEW_MATRIX, trackball.modelview);
    update_projection_matrix();
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

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::push_obj(const DrawableObject *obj, bool refit_scene)
{
    if (obj==NULL) return;
    objects.push_back(obj);
    if (refit_scene) fit_scene();
    updateGL();
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
    for(std::vector<const DrawableObject*>::iterator it=objects.begin(); it!=objects.end(); ++it)
    {
        const DrawableObject *obj = *it;

        if (obj->object_type() == type)
        {
            objects.erase(it);
            return true;
        }
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool GLcanvas::pop(const DrawableObject *obj)
{
    for(std::vector<const DrawableObject*>::iterator it=objects.begin(); it!=objects.end(); ++it)
    {
        if (obj == *it)
        {
            objects.erase(it);
            return true;
        }
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::push_label(const uint x, const uint y, const std::string & label)
{
    TextLabel l;
    l.is_3d = false;
    l.x     = x;
    l.y     = y;
    l.label = label;
    labels.push_back(l);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::push_label(const vec3d & p, const std::string & label)
{
    TextLabel l;
    l.is_3d = true;
    l.xyz   = p;
    l.label = label;
    labels.push_back(l);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::pop_label()
{
    labels.pop_back();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::pop_all_labels()
{
    labels.clear();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::wheelEvent(QWheelEvent *event) // zoom
{
    event->accept();
    float d = -(float)event->delta()/120.0*0.2*trackball.scene_size;
    // coming closer would make depth values unstable
    if(d>trackball.z_near) d = trackball.z_near - 1e-3;
    translate(vec3d(0,0,d));
    trackball.z_near -= d;
    trackball.z_far  -= d;
    update_projection_matrix();
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::set_scene_center(const vec3d & center)
{
    // https://stackoverflow.com/questions/15697273/how-can-i-get-view-direction-from-the-opengl-modelview-matrix
    // http://blog.db-in.com/cameras-on-opengl-es-2-x/
    //
    // Right Vector => MV[0], MV[4], MV[8]
    // Up    Vector => MV[1], MV[5], MV[9]
    // Look  Vector => MV[2], MV[6], MV[10]
    //
    // MV[12] => Tx
    // MV[13] => Ty
    // MV[14] => Tz
    //
    double dx = trackball.modelview[ 0]*center[0] +
                trackball.modelview[ 4]*center[1] +
                trackball.modelview[ 8]*center[2] +
                trackball.modelview[12];
    double dy = trackball.modelview[ 1]*center[0] +
                trackball.modelview[ 5]*center[1] +
                trackball.modelview[ 9]*center[2] +
                trackball.modelview[13];
    double dz = trackball.modelview[ 2]*center[0] +
                trackball.modelview[ 6]*center[1] +
                trackball.modelview[10]*center[2] +
                trackball.modelview[14]+3.0*trackball.scene_size;
                // without this latter sum the scene would be centered exactly
                // at the camera. This shifts the center of the scene away along
                // the Look vector by a factor proportional to the scene size.

    translate(vec3d(-dx,-dy,-dz));

    // keep distance between z_far and z_near as short as possible
    trackball.z_near       = 2.5*trackball.scene_size;
    trackball.z_far        = 3.5*trackball.scene_size;
    trackball.scene_center = center;
    update_projection_matrix();
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::fit_scene()
{
    vec3d center(0,0,0);
    float size  = 0.0;
    uint  count = 0;
    for(const DrawableObject *obj : objects)
    {
        center += obj->scene_center();
        size  = std::max(size, obj->scene_radius());
        ++count;
    }
    if(count==0||size==0) return;
    center /= (double)count;

    trackball.scene_size = size;
    set_scene_center(center);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::updateGL()
{
    update();
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
    vec3d O = trackball.scene_center;
    vec3d X = trackball.scene_center + vec3d(1,0,0)*trackball.scene_size;
    vec3d Y = trackball.scene_center + vec3d(0,1,0)*trackball.scene_size;
    vec3d Z = trackball.scene_center + vec3d(0,0,1)*trackball.scene_size;

    makeCurrent();
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_DEPTH_TEST);
    cylinder(O, X, trackball.scene_size*0.015, trackball.scene_size*0.015, Color::RED().rgba);
    cylinder(O, Y, trackball.scene_size*0.015, trackball.scene_size*0.015, Color::GREEN().rgba);
    cylinder(O, Z, trackball.scene_size*0.015, trackball.scene_size*0.015, Color::BLUE().rgba);
    sphere(O, trackball.scene_size*0.02, Color::WHITE().rgba);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
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
    p3d[2] = (x2y2 < 0.5*trackball.radius) ? sqrt(trackball.radius - x2y2) : 0.5*trackball.radius/sqrt(x2y2);
}

}
