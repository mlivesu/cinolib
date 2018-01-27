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
#include <cinolib/gui/qt/glcanvas2.h>
#include <cinolib/cino_inline.h>
#include <cinolib/gl/draw_sphere.h>
#include <cinolib/gl/draw_cylinder.h>
#include <cinolib/color.h>

#include <sstream>
#include <QApplication>
#include <QColorDialog>
#include <QShortcut>
#include <QMouseEvent>
#include <QMimeData>
#include <QMenu>

namespace cinolib
{

CINO_INLINE
GLcanvas2::GLcanvas2(QWidget * parent) : QOpenGLWidget(parent), QOpenGLFunctions()
{
    scene_radius = 1.0;
    scene_center = vec3d(0,0,0);
    render_axis  = true;

    // enables depth buffer
    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(32);
    format.setSamples(0);
    setFormat(format);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas2::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.0,1.0,1.0,1.0);

    makeCurrent();
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST); // this should go elsewhere! (cinolib/gl/draw_lines_tris)
    glEnable(GL_COLOR_MATERIAL);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // scene pos and size
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetDoublev(GL_MODELVIEW_MATRIX, trackball.modelview);
    update_projection_matrix();
    fit_scene();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void GLcanvas2::resizeGL(int w, int h)
{
    makeCurrent();
    update_projection_matrix();
    glViewport(0, 0, w, h);
    update();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas2::paintGL()
{
    makeCurrent();
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    for(auto obj : drawlist) obj->draw(scene_radius);

    for(auto p : points) sphere(p, scene_radius*0.07, Color::BLACK().rgba);

    if (render_axis)
    {
        vec3d O = scene_center;  //(0,0,0);
        vec3d X = scene_center + vec3d(1,0,0)*scene_radius;
        vec3d Y = scene_center + vec3d(0,1,0)*scene_radius;
        vec3d Z = scene_center + vec3d(0,0,1)*scene_radius;
        cylinder(O, X, scene_radius*0.015, scene_radius*0.015, Color::RED().rgba);
        cylinder(O, Y, scene_radius*0.015, scene_radius*0.015, Color::GREEN().rgba);
        cylinder(O, Z, scene_radius*0.015, scene_radius*0.015, Color::BLUE().rgba);
        sphere(O, scene_radius*0.02, Color::WHITE().rgba);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas2::fit_scene()
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

    std::cout << std::endl;
    std::cout << "Scene Center: " << scene_center << std::endl;
    std::cout << "Scene Radius: " << scene_radius << std::endl;
    std::cout << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas2::updateGL()
{
    update();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas2::update_projection_matrix()
{
    makeCurrent();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)width()/(GLfloat)height(), 0.5*scene_radius, 10.0*scene_radius);
    glGetDoublev(GL_PROJECTION_MATRIX, trackball.projection);
    glMatrixMode(GL_MODELVIEW);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// This is actually doing the Sphere/Hyperbolic sheet hybrid thing,
// based on Ken Shoemake's ArcBall in Graphics Gems IV, 1993.
//
CINO_INLINE
void GLcanvas2::map_to_sphere(const QPoint & p2d, vec3d & p3d)
{
    double x =  (2.0 *  p2d.x() - width()) / width();
    double y = -(2.0 *  p2d.y() - height()) / height();
    double xval = x;
    double yval = y;
    double x2y2 = xval * xval + yval * yval;

    p3d[0] = xval;
    p3d[1] = yval;
    p3d[2] = (x2y2 < 0.5*trackball.radius) ? sqrt(trackball.radius - x2y2) : 0.5*trackball.radius/sqrt(x2y2);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas2::translate(const vec3d & t)
{
    makeCurrent();
    glLoadIdentity();
    glTranslated(t[0], t[1], t[2]);
    glMultMatrixd(trackball.modelview);
    glGetDoublev(GL_MODELVIEW_MATRIX, trackball.modelview);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas2::rotate(const vec3d & axis, const double angle)
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
void GLcanvas2::push_obj(const DrawableObject *obj, bool refit_scene)
{
    drawlist.push_back(obj);
    if (refit_scene) fit_scene();
    update();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool GLcanvas2::pop_all_occurrences_of(int type)
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
bool GLcanvas2::pop_first_occurrence_of(int type)
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
bool GLcanvas2::pop(const DrawableObject *obj)
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
void GLcanvas2::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    trackball.mouse_pressed = true;
    trackball.last_point_2d = event->pos();
    map_to_sphere(trackball.last_point_2d, trackball.last_point_3d);





//      QOpenGLContext *ctx = QOpenGLContext::currentContext();

//      QOpenGLFramebufferObjectFormat format;
//      format.setSamples(0);
//      format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
//      QOpenGLFrameBufferObject *mFBO = new QOpenGLFramebufferObject(size(), format);

//      glBindFramebuffer(GL_READ_FRAMEBUFFER, defaultFramebufferObject());
//      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO->handle());
//      ctx->extraFunctions()->glBlitFramebuffer(0, 0, width(), height(), 0, 0, mFBO->width(), mFBO->height(), GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT, GL_NEAREST);

//      mFBO->bind(); // must rebind, otherwise it won't work!

//      float mouseDepth = 1.f;
//      glReadPixels(50, 50, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &mouseDepth);

//      mFBO->release();
//      delete mFBO;




    // https://www.opengl.org/discussion_boards/showthread.php/145308-Depth-Buffer-How-do-I-get-the-pixel-s-Z-coord
    QPoint  click = event->pos();
    GLfloat depth_comp;

    GLfloat winX = click.x();
    GLfloat winY = -click.y() + height();
    glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth_comp);
    std::cout << "click: " << winX << "  " << winY << std::endl;
    std::cout << "depth val: " << depth_comp << std::endl<< std::endl;

//    GLfloat clip_z  = (depth_comp - 0.5) * 2.0;
//    GLfloat near_z  = 0.5*scene_radius;
//    GLfloat far_z   = 10.0*scene_radius;
//    GLfloat world_z = 2.0*far_z*near_z/(clip_z*(far_z-near_z)-(far_z+near_z));
//    //far_z*near_z/(far_z-clip_z*(far_z-near_z));
//    // 2*far_z*near_z/(clip_z*(far_z-near_z)-(far_z+near_z));

//    double mv[16], pr[16];
//    int vp[4];
//    glGetDoublev( GL_MODELVIEW_MATRIX, mv);
//    glGetDoublev( GL_PROJECTION_MATRIX, pr);
//    glGetIntegerv( GL_VIEWPORT, vp);
//    vec3d p;
//    gluUnProject(click.x(), click.y(), world_z, mv, pr, vp, &p.x(), &p.y(), &p.z());

////    std::cout << "depth val in -1 1: " << clip_z << std::endl;
////    std::cout << "near_z: " << near_z << std::endl;
////    std::cout << "far_z: " << far_z << std::endl;
////    std::cout << p << std::endl;
//    points.push_back(p);

////    QPoint  click = event->pos();
////    float x = click.x();
////    float y = click.y();
////    GLint viewport[4];
////    GLdouble modelview[16];
////    GLdouble projection[16];
////    GLfloat winX, winY, winZ;
////    GLdouble posX, posY, posZ;
////    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
////    glGetDoublev( GL_PROJECTION_MATRIX, projection );
////    glGetIntegerv( GL_VIEWPORT, viewport );
////    winX = (float)x;
////    winY = /*(float)viewport[3] -*/ (float)y;
////    glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
////    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
////    points.push_back(vec3d(posX,posY,posZ));
////    std::cout << points.back() << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas2::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    trackball.mouse_pressed = false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas2::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();

    // translate
    if (event->modifiers() & Qt::ShiftModifier)
    {
        QPoint p2d = event->pos();
        float  dx  = p2d.x() - trackball.last_point_2d.x();
        float  dy  = p2d.y() - trackball.last_point_2d.y();
        float  z   = - (trackball.modelview[ 2]*scene_center[0] +
                        trackball.modelview[ 6]*scene_center[1] +
                        trackball.modelview[10]*scene_center[2] +
                        trackball.modelview[14]) /
                       (trackball.modelview[ 3]*scene_center[0] +
                        trackball.modelview[ 7]*scene_center[1] +
                        trackball.modelview[11]*scene_center[2] +
                        trackball.modelview[15]);

        float w          = width();
        float h          = height();
        float aspect     = w/h;
        float near_plane = 0.01 * scene_radius;
        float top        = tan(45.f/2.0f*M_PI/180.0f) * near_plane;
        float right      = aspect*top;

        translate(vec3d(2.0*dx/w*right/near_plane*z, -2.0*dy/h*top/near_plane*z, 0.0f));
        trackball.last_point_2d = p2d;
        update();
        return;
    }

    // rotate
    if (event->buttons() == Qt::LeftButton)
    {
        QPoint p2d = event->pos();
        vec3d  p3d;
        map_to_sphere(p2d, p3d);
        if (trackball.mouse_pressed)
        {
            vec3d axis = trackball.last_point_3d.cross(p3d);

            if (axis.length_squared()<1e-7) axis = vec3d(1,0,0);
            axis.normalize();

            // find the amount of rotation
            vec3d d = trackball.last_point_3d - p3d;
            float t = 0.5*d.length()/trackball.radius;

            if      (t < -1.0)  t = -1.0;
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
void GLcanvas2::wheelEvent(QWheelEvent *event)
{
    event->accept();
    float d = -(float) event->delta()/120.0*0.2*scene_radius;
    translate(vec3d(0,0,d));
    // Jan 27, 2018: maybe move clipping planes to avoid front clipping?
    update();
}

}
