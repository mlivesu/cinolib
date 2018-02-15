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
    make_popup_menu();
    setFocusPolicy(Qt::StrongFocus);

    clear_color = QColor(200, 200, 200);
    font        = QFont("Courier New", 14);
    show_helper = false;
    show_axis   = false;

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
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    reset_trackball();
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
    for(auto obj:objects) obj->draw(trackball.scene_size);

    // render axis/labels/helper
    if (show_axis)     draw_axis();
    for(auto l:labels) draw_text(l);
    draw_helper();
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
void GLcanvas::zoom(double d)
{
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
void GLcanvas::rotate(const vec3d & axis, const double angle)
{
    vec3d t(trackball.modelview[ 0]*trackball.pivot[0] +
            trackball.modelview[ 4]*trackball.pivot[1] +
            trackball.modelview[ 8]*trackball.pivot[2] +
            trackball.modelview[12],
            trackball.modelview[ 1]*trackball.pivot[0] +
            trackball.modelview[ 5]*trackball.pivot[1] +
            trackball.modelview[ 9]*trackball.pivot[2] +
            trackball.modelview[13],
            trackball.modelview[ 2]*trackball.pivot[0] +
            trackball.modelview[ 6]*trackball.pivot[1] +
            trackball.modelview[10]*trackball.pivot[2] +
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
        case Qt::Key_H:     show_helper=!show_helper;                 break;
        case Qt::Key_A:     show_axis=!show_axis;                     break;
        case Qt::Key_R:     reset_trackball();                        break;
        case Qt::Key_C:     trackball.pivot = trackball.scene_center; break;
        case Qt::Key_Left:  rotate(vec3d(0,1,0), -3);                 break;
        case Qt::Key_Right: rotate(vec3d(0,1,0), +3);                 break;
        case Qt::Key_Up:    rotate(vec3d(1,0,0), -3);                 break;
        case Qt::Key_Down:  rotate(vec3d(1,0,0), +3);                 break;
    }
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::mouseDoubleClickEvent(QMouseEvent *event)
{
    event->accept();
    vec3d new_pivot;
    if (unproject(event->pos(), new_pivot)) set_rotation_pivot(new_pivot);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button()    == Qt::RightButton &&
        event->modifiers() == Qt::ControlModifier)
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
    if (event->buttons() == Qt::RightButton)
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
void GLcanvas::push_label(const vec2i & p, const std::string & label, const Color & c)
{
    TextLabel l;
    l.label = label;
    l.is_3d = false;
    l.p2d   = p;
    l.color = c;
    labels.push_back(l);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::push_label(const vec3d & p, const std::string & label, const Color & c)
{
    TextLabel l;
    l.label = label;
    l.is_3d = true;
    l.p3d   = p;
    l.color = c;
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
    zoom(d);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::set_rotation_pivot(const vec3d & new_pivot)
{
    trackball.pivot = new_pivot;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::set_scene_center(const vec3d  & new_center,
                                const double   dist_from_camera,
                                const bool     pivot_at_center)
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
    double dx = trackball.modelview[ 0]*new_center[0] +
                trackball.modelview[ 4]*new_center[1] +
                trackball.modelview[ 8]*new_center[2] +
                trackball.modelview[12];
    double dy = trackball.modelview[ 1]*new_center[0] +
                trackball.modelview[ 5]*new_center[1] +
                trackball.modelview[ 9]*new_center[2] +
                trackball.modelview[13];
    double dz = trackball.modelview[ 2]*new_center[0] +
                trackball.modelview[ 6]*new_center[1] +
                trackball.modelview[10]*new_center[2] +
                trackball.modelview[14]+dist_from_camera;

    translate(vec3d(-dx,-dy,-dz)); // move camera "dist_from_camera" from center (along Z)

    trackball.scene_center = new_center;
    trackball.z_far        = dist_from_camera + trackball.scene_size;
    trackball.z_near       = dist_from_camera - trackball.scene_size;
    if(trackball.z_near<1e-3) trackball.z_near = 1e-3; // avoid <=0 z_near
    if(pivot_at_center)       set_rotation_pivot(new_center);
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
    set_scene_center(center, 3.0*trackball.scene_size, true);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::updateGL()
{
    update();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
int GLcanvas::height_retina()
{
    // http://doc.qt.io/qt-5/qwindow.html#devicePixelRatio
    return height() * devicePixelRatio();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
int GLcanvas::width_retina()
{
    // http://doc.qt.io/qt-5/qwindow.html#devicePixelRatio
    return width() * devicePixelRatio();
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
    vec3d  O = trackball.scene_center;
    vec3d  X = trackball.scene_center + vec3d(1,0,0)*trackball.scene_size;
    vec3d  Y = trackball.scene_center + vec3d(0,1,0)*trackball.scene_size;
    vec3d  Z = trackball.scene_center + vec3d(0,0,1)*trackball.scene_size;
    double r = trackball.scene_size*0.01;

    makeCurrent();
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_DEPTH_TEST);
    cylinder(O, X, r, r, Color::RED().rgba);
    cylinder(O, Y, r, r, Color::GREEN().rgba);
    cylinder(O, Z, r, r, Color::BLUE().rgba);
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

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool GLcanvas::unproject(const QPoint & click, vec3d & p)
{
    makeCurrent();

    // accout for retina displays when reading buffers
    // http://doc.qt.io/qt-5/qwindow.html#devicePixelRatio
    //
    GLint x = click.x() * devicePixelRatio();
    GLint y = click.y() * devicePixelRatio();
    GLfloat depth;
    glReadPixels(x, height_retina()-1-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

    if (depth >= 1)
    {
        std::cout << "Unproject click(" << click.x() << "," << click.y() << ") depth: -1 [failed]" << std::endl;
        return false;
    }

    GLint viewport[4] =
    {
        0,        height(), // top left corner
        width(), -height()  // bottom right corner
    };

    gluUnProject(click.x(), click.y(), depth, trackball.modelview, trackball.projection, viewport,  &p.x(), &p.y(), &p.z());

    std::cout << "Unproject click(" << click.x() << "," << click.y() << ") depth: " << depth << " => " << p <<std::endl;
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::draw_helper()
{
    vec2i p(10,25);
    uint  step = 17; // line spacing
    glColor3f(0,0,0);
    if (show_helper)
    {
        draw_text(p, "Left  but       : rotate         "); p.y()+=step;
        draw_text(p, "Right but       : translate      "); p.y()+=step;
        draw_text(p, "Double click    : change pivot   "); p.y()+=step;
        draw_text(p, "Key C           : pivot at center"); p.y()+=step;
        draw_text(p, "Key R           : reset trackball"); p.y()+=step;
        draw_text(p, "Key A           : toggle axis    "); p.y()+=step;
        draw_text(p, "Key H           : toggle helper  "); p.y()+=step;
        draw_text(p, "Key Left        : rotate left    "); p.y()+=step;
        draw_text(p, "Key Right       : rotate right   "); p.y()+=step;
        draw_text(p, "Key Up          : rotate up      "); p.y()+=step;
        draw_text(p, "Key Down        : rotate down    "); p.y()+=step;
        draw_text(p, "Cmd + Key C     : copy  POV      "); p.y()+=step;
        draw_text(p, "Cmd + Key V     : paste POV      "); p.y()+=step;
        draw_text(p, "Cmd + Right but : popup menu     "); p.y()+=step;
        // add +/shift+ to move znear
        // add -/shift- to move zfar
    }
    else
    {
        draw_text(p, "Key H: toggle helper");
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::draw_text(const TextLabel & t)
{
    if (t.is_3d) draw_text(t.p3d, t.label.c_str(), t.color);
    else         draw_text(t.p2d, t.label.c_str(), t.color);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::draw_text(const vec3d & pos, const std::string & text, const Color & c)
{
    glColor3fv(c.rgba);
    renderText(pos.x(), pos.y(), pos.z(), text.c_str(), font);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::draw_text(const vec2i & pos, const std::string & text, const Color & c)
{
    glColor3fv(c.rgba);
    renderText(pos.x(), pos.y(), text.c_str(), font);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::reset_trackball()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetDoublev(GL_MODELVIEW_MATRIX, trackball.modelview);
    update_projection_matrix();
    fit_scene();
}

}
