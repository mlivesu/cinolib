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
#include <cinolib/gui/qt/glcanvas.h>
#include <cinolib/cino_inline.h>
#include <cinolib/gl/draw_sphere.h>
#include <cinolib/gl/draw_cylinder.h>
#include <cinolib/color.h>
#include <cinolib/pi.h>

#include <sstream>
#include <chrono>
#include <thread>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QShortcut>
#include <QMouseEvent>
#include <QMimeData>

namespace cinolib
{

CINO_INLINE
GLcanvas::GLcanvas(QWidget *parent) : QGLWidget(parent)
{
    make_popup_menu();
    setFocusPolicy(Qt::StrongFocus);

    timer_id    = 0;
    clear_color = QColor(200, 200, 200);
    show_helper = false;
    show_axis   = false;
    show_pivot  = false;

    // initialize callbacks
    callback_key_press          = nullptr;
    callback_mouse_double_click = nullptr;
    callback_mouse_press        = nullptr;
    callback_mouse_release      = nullptr;
    callback_mouse_move         = nullptr;
    callback_wheel              = nullptr;

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
    pop_all_markers();
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

    // For compatibility between native GL code and QPainter, see (end of the page)
    // http://www.informit.com/articles/article.aspx?p=1405557&seqNum=2
    setAutoFillBackground(false);
    setAutoBufferSwap(false);

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
    // --------------- NATIVE OPEN GL RENDERING ---------------- //

    glClearColor(clear_color.redF(), clear_color.greenF(), clear_color.blueF(), clear_color.alphaF());
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // render drawable objects (meshes, curve-skeletons, ...)
    for(auto obj : objects) obj->draw(trackball.scene_size);

    // render global XYZ axis
    if (show_axis) draw_axis();

    // ------ QPAINTER-BASED RENDERING (text,circles,...) ------ //

    QPainter painter(this);

    // render markers
    for(auto l : markers) draw_marker(painter, *l);

    // render center of trackball (pivot)
    if (show_pivot)
    {
        Marker m;
        m.p3d   = trackball.pivot;
        m.label = "pivot";
        draw_marker(painter, m);
    }

    // render helper
    draw_helper(painter);

    swapBuffers();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::update_projection_matrix()
{
    makeCurrent();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat ratio = (GLfloat)width()/(GLfloat)height();
    if(trackball.perspective)
    {
        gluPerspective(trackball.zoom_persp, ratio, trackball.z_near, trackball.z_far);
    }
    else
    {
        glOrtho(-trackball.scene_size*trackball.zoom_ortho*ratio,
                 trackball.scene_size*trackball.zoom_ortho*ratio,
                -trackball.scene_size*trackball.zoom_ortho,
                 trackball.scene_size*trackball.zoom_ortho,
                 trackball.z_near,
                 trackball.z_far);
    }
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
void GLcanvas::zoom(const double angle)
{   
    trackball.zoom_persp += angle;
    trackball.zoom_ortho += angle*0.01;

    if(trackball.zoom_persp < 1e-5 ) trackball.zoom_persp = 1e-5f;
    if(trackball.zoom_persp > 179.0) trackball.zoom_persp = 179.0f;
    if(trackball.zoom_ortho < 1e-5 ) trackball.zoom_ortho = 1e-5f;

    update_projection_matrix();
    updateGL();
    return;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::rotate(const vec3d & axis, const double angle)
{
    if(trackball.mode_2d) return; // disable rotations in 2D mode

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
    if(callback_key_press) callback_key_press(this, event);
    if(skip_default_keypress_handler) return;

    switch (event->key())
    {
        case Qt::Key_H:     show_helper=!show_helper;               break;
        case Qt::Key_A:     show_axis=!show_axis;                   break;
        case Qt::Key_S:     show_pivot=!show_pivot;                 break;
        case Qt::Key_R:     reset_trackball();                      break;
        case Qt::Key_C:     trackball.pivot=trackball.scene_center; break;
        case Qt::Key_O:     animation_orbit();                      break;
        case Qt::Key_Left:  rotate(vec3d(0,1,0), -2);               break;
        case Qt::Key_Right: rotate(vec3d(0,1,0), +2);               break;
        case Qt::Key_Up:    rotate(vec3d(1,0,0), -2);               break;
        case Qt::Key_Down:  rotate(vec3d(1,0,0), +2);               break;
        case Qt::Key_P:     trackball.perspective=!trackball.perspective;
                            update_projection_matrix();             break;
    }
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(callback_mouse_double_click) callback_mouse_double_click(this, event);
    if(skip_default_mouse_double_click_handler) return;

    vec3d new_pivot;
    vec2i click(event->x(), event->y());
    if (unproject(click, new_pivot))
    {
        set_rotation_pivot(new_pivot);
        //show_pivot = true;
    }
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::mousePressEvent(QMouseEvent *event)
{
    if(callback_mouse_press) callback_mouse_press(this, event);
    if(skip_default_mouse_press_handler) return;

    if(event->button()    == Qt::RightButton &&
       event->modifiers() == Qt::ControlModifier)
    {
        popup->exec(QCursor::pos());
    }
    else
    {
        trackball.mouse_pressed = true;
        trackball.last_click_2d = event->pos();
        map_to_sphere(trackball.last_click_2d, trackball.last_click_3d);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if(callback_mouse_release) callback_mouse_release(this, event);
    if(skip_default_mouse_release_handler) return;
    trackball.mouse_pressed = false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::mouseMoveEvent(QMouseEvent *event)
{
    if(callback_mouse_move) callback_mouse_move(this, event);
    if(skip_default_mouse_move_handler) return;

    // translate
    if(event->buttons() == Qt::RightButton)
    {
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
    if(event->buttons() == Qt::LeftButton)
    {
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
void GLcanvas::wheelEvent(QWheelEvent *event) // zoom
{
    if(callback_wheel) callback_wheel(this, event);
    if(skip_default_wheel_handler) return;

    zoom(event->delta()/8.f);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::timerEvent(QTimerEvent *event) // refresh canvas
{
    if(callback_timer) callback_timer(this, event);
    if(skip_default_timer_handler) return;

    updateGL_strict();
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
       << trackball.z_near        << " " << trackball.z_far         << " "
       << trackball.perspective   << " " << trackball.zoom_persp    << " " << trackball.zoom_ortho;
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
       >> trackball.z_near        >> trackball.z_far
       >> trackball.perspective   >> trackball.zoom_persp    >> trackball.zoom_ortho;
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
void GLcanvas::push_marker(const Marker * m)
{
    markers.push_back(m);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::push_marker(const vec2i & p, const std::string & label, const Color & c, const uint font_size, const uint disk_size)
{
    Marker *l     = new Marker;
    l->p2d        = p;
    l->label      = label;
    l->font_size  = font_size;
    l->disk_size  = disk_size;
    l->color      = c;
    markers.push_back(l);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::push_marker(const vec3d & p, const std::string & label, const Color & c, const uint font_size, const uint disk_size)
{
    Marker *l     = new Marker;
    l->p3d        = p;
    l->label      = label;
    l->font_size  = font_size;
    l->disk_size  = disk_size;
    l->color      = c;
    markers.push_back(l);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::pop_marker()
{
    delete markers.back();
    markers.pop_back();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::pop_all_markers()
{
    for(auto *ptr : markers) delete ptr;
    markers.clear();
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
    if(trackball.z_near<1e-3) trackball.z_near = 1e-3f; // avoid <=0 z_near
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
        if(obj->scene_radius()==0) continue;

        center += obj->scene_center();
        size  = std::max(size, obj->scene_radius());
        ++count;
    }
    if(count==0||size==0) return;
    center /= (double)count;
    double dist = 0;
    for(const DrawableObject *obj : objects)
    {
        dist = std::max(dist, obj->scene_center().dist(center));
    }
    trackball.scene_size = size + dist;
    set_scene_center(center, 3.0*trackball.scene_size, true);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::updateGL()
{
    // schedule rendering
    update();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::updateGL_strict()
{
    // force immediate rendering
    // http://doc.qt.io/qt-5/qwidget.html#repaint
    repaint();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double GLcanvas::scene_size() const
{
    return trackball.scene_size;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d GLcanvas::scene_center() const
{
    return trackball.scene_center;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::set_2d_mode(const bool b)
{
    trackball.mode_2d = b;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::animation_start(const int period_ms)
{
    // WARNING: for single-threaded applications that run both
    // the GUI and the algorithms in the same thread, refresh
    // will not occur until the algorithm returns to the main
    // application loop. This may result in unresponsive GUIs
    //
    timer_id = startTimer(period_ms); // milliseconds
    if (timer_id == 0) std::cout << "WARNING: could not start animation timer" << std::endl;
    else               std::cout << "Start animation timer " << timer_id << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::animation_stop()
{
    std::cout << "Stop animation timer " << timer_id << std::endl;
    killTimer(timer_id);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::animation_orbit(const vec3d & axis, const uint step)
{
    for(uint alpha=0; std::fabs(alpha)<360; alpha+=step)
    {
        rotate(axis,step);
        updateGL_strict();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
int GLcanvas::height_retina() const
{
    // http://doc.qt.io/qt-5/qwindow.html#devicePixelRatio
    return height() * devicePixelRatio();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
int GLcanvas::width_retina() const
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
bool GLcanvas::read_Z_buffer(const vec2i & p2d, GLfloat & depth)
{
    makeCurrent(); // violates const

    // accout for retina displays when reading buffers
    // http://doc.qt.io/qt-5/qwindow.html#devicePixelRatio
    //
    GLint x = p2d.x() * devicePixelRatio();
    GLint y = p2d.y() * devicePixelRatio();
    glReadPixels(x, height_retina()-1-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

    if (depth >= 1 || depth <= 0) return false;
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool GLcanvas::unproject(const vec2i & p2d, vec3d & p3d)
{
    GLfloat depth;
    if(!read_Z_buffer(p2d, depth))
    {
        //std::cout << "Unproject click(" << p2d.x() << "," << p2d.y() << ") depth: 1 [failed]" << std::endl;
        return false;
    }

    GLint viewport[4] =
    {
        0,        height(), // top left corner
        width(), -height()  // bottom right corner
    };

    gluUnProject(p2d.x(), p2d.y(), depth, trackball.modelview, trackball.projection, viewport,  &p3d.x(), &p3d.y(), &p3d.z());

    //std::cout << "Unproject click(" << p2d.x() << "," << p2d.y() << ") depth: " << depth << " => " << p3d <<std::endl;
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool GLcanvas::project(const vec3d & p3d, vec2i & p2d, GLdouble & depth)
{
    makeCurrent(); // violates const

    GLint viewport[4] =
    {
        0,        height(), // top left corner
        width(), -height()  // bottom right corner
    };

    GLdouble winX, winY;
    gluProject(p3d.x(), p3d.y(), p3d.z(), trackball.modelview, trackball.projection, viewport,  &winX, &winY, &depth);

    p2d.x() = static_cast<int>(winX);
    p2d.y() = static_cast<int>(winY);

    if(winX>=0 && winX<=width() && winY>=0 && winY<=height()) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if p3d stays in front of what is currently stored in the depth buffer
//
CINO_INLINE
bool GLcanvas::depth_test(const vec3d & p3d)
{
    GLdouble depth;
    vec2i    p2d;
    project(p3d, p2d, depth);

    GLfloat z_buff;
    if (!read_Z_buffer(p2d, z_buff)) return true; // if reading the z-buffer fails, the test is passed by default
    if (z_buff >= depth) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::draw_helper(QPainter & painter)
{
    vec2i p(10,25);
    uint  step    = 17;  // line   spacing
    uint  fir_col = 10;  // first  text column
    uint  sec_col = 111; // second text column
    if(show_helper)
    {
                       draw_text(painter, p, "Left  but      ", 12); p.x()=sec_col; draw_text(painter, p, ": rotate          ", 12); p.y()+=step;
        p.x()=fir_col; draw_text(painter, p, "Right but      ", 12); p.x()=sec_col; draw_text(painter, p, ": translate       ", 12); p.y()+=step;
        p.x()=fir_col; draw_text(painter, p, "Double click   ", 12); p.x()=sec_col; draw_text(painter, p, ": change pivot    ", 12); p.y()+=step;
        p.x()=fir_col; draw_text(painter, p, "Key C          ", 12); p.x()=sec_col; draw_text(painter, p, ": pivot at center ", 12); p.y()+=step;
        p.x()=fir_col; draw_text(painter, p, "Key S          ", 12); p.x()=sec_col; draw_text(painter, p, ": show pivot      ", 12); p.y()+=step;
        p.x()=fir_col; draw_text(painter, p, "Key R          ", 12); p.x()=sec_col; draw_text(painter, p, ": reset trackball ", 12); p.y()+=step;
        p.x()=fir_col; draw_text(painter, p, "Key A          ", 12); p.x()=sec_col; draw_text(painter, p, ": toggle axis     ", 12); p.y()+=step;
        p.x()=fir_col; draw_text(painter, p, "Key H          ", 12); p.x()=sec_col; draw_text(painter, p, ": toggle helper   ", 12); p.y()+=step;
        p.x()=fir_col; draw_text(painter, p, "Key O          ", 12); p.x()=sec_col; draw_text(painter, p, ": orbit animation ", 12); p.y()+=step;
        p.x()=fir_col; draw_text(painter, p, "Key P          ", 12); p.x()=sec_col; draw_text(painter, p, ": persp/ortho view", 12); p.y()+=step;
        p.x()=fir_col; draw_text(painter, p, "Key Left       ", 12); p.x()=sec_col; draw_text(painter, p, ": rotate left     ", 12); p.y()+=step;
        p.x()=fir_col; draw_text(painter, p, "Key Right      ", 12); p.x()=sec_col; draw_text(painter, p, ": rotate right    ", 12); p.y()+=step;
        p.x()=fir_col; draw_text(painter, p, "Key Up         ", 12); p.x()=sec_col; draw_text(painter, p, ": rotate up       ", 12); p.y()+=step;
        p.x()=fir_col; draw_text(painter, p, "Key Down       ", 12); p.x()=sec_col; draw_text(painter, p, ": rotate down     ", 12); p.y()+=step;
        p.x()=fir_col; draw_text(painter, p, "Cmd + Key C    ", 12); p.x()=sec_col; draw_text(painter, p, ": copy  POV       ", 12); p.y()+=step;
        p.x()=fir_col; draw_text(painter, p, "Cmd + Key V    ", 12); p.x()=sec_col; draw_text(painter, p, ": paste POV       ", 12); p.y()+=step;
        p.x()=fir_col; draw_text(painter, p, "Cmd + Right but", 12); p.x()=sec_col; draw_text(painter, p, ": popup menu      ", 12);
        // add +/shift+ to move znear
        // add -/shift- to move zfar
    }
    else
    {
        draw_text(painter, p, "H for help", 12);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::draw_marker(QPainter & painter, const Marker & t)
{
    vec2i p = t.p2d;
    if (p.x()<0) // 2D coordinates point offscreen, use the projection of 3D point instead
    {
        GLdouble depth;
        project(t.p3d, p, depth);
    }

    if(t.disk_size>0)
    {
        draw_disk(painter, p, t.disk_size, t.color);
        draw_text(painter, p + vec2i(0,-2), t.label, t.font_size, t.color); // shift text to detach it from the disk
    }
    else draw_text(painter, p, t.label, t.font_size, t.color);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::draw_text(QPainter & painter, const vec2i & pos, const std::string & text, const uint font_size, const Color & c)
{
    if (text.empty()) return;
    painter.setPen(QColor(c.r_uchar(), c.g_uchar(), c.b_uchar()));
    painter.setFont(QFont("Helvetica",font_size));
    painter.drawText(pos.x(), pos.y(), text.c_str());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::draw_disk(QPainter & painter, const vec2i & center, const uint radius, const Color & c)
{
    if (radius<1) return;
    painter.setPen(QColor(c.r_uchar(), c.g_uchar(), c.b_uchar()));
    painter.setBrush(QColor(c.r_uchar(), c.g_uchar(), c.b_uchar()));
    painter.drawEllipse(center.x(), center.y(), radius, radius);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::reset_trackball()
{
    makeCurrent();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetDoublev(GL_MODELVIEW_MATRIX, trackball.modelview);

    trackball.mode_2d     = false;
    trackball.perspective = true;
    trackball.zoom_ortho  = 1.0;
    trackball.zoom_persp  = 45.0;
    update_projection_matrix();
    fit_scene();
}

}
