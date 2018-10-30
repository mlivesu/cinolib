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
GLcanvas::GLcanvas(QWidget * parent)
{
    setParent(parent);
    make_popup_menu();

    clear_color = QColor(200, 200, 200);

    // enable cut/paste shortcuts to copy/paste points of view for fast reproduction of paper images/comparisons
    //
    connect(new QShortcut(QKeySequence::Copy, this), &QShortcut::activated, [&](){
        QApplication::clipboard()->setText(serialize_camera().data());
    });
    //
    connect(new QShortcut(QKeySequence::Paste, this), &QShortcut::activated, [&](){
        if (QApplication::clipboard()->mimeData()->hasText())
        {
            deserialize_camera(QApplication::clipboard()->mimeData()->text().toStdString());
        }
    });
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::make_popup_menu()
{
    popup = new QMenu(this);

    QAction *background_color = new QAction("Brackground color", this);
    connect(background_color, &QAction::triggered, [&]() {
        this->set_clear_color(QColorDialog::getColor(Qt::white, this));
        updateGL();
    });
    popup->addAction(background_color);

    QAction *copy_POV = new QAction("Copy POV", this);
    connect(copy_POV, &QAction::triggered, [&]() {
        QApplication::clipboard()->setText(serialize_camera().data());
    });
    popup->addAction(copy_POV);

    QAction *paste_POV = new QAction("Paste POV", this);
    connect(paste_POV, &QAction::triggered, [&]() {
        if (QApplication::clipboard()->mimeData()->hasText())
        {
            deserialize_camera(QApplication::clipboard()->mimeData()->text().toStdString());
        }
    });
    popup->addAction(paste_POV);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button()   == Qt::RightButton &&
        event->buttons()  == Qt::RightButton &&
        event->modifiers()== Qt::ControlModifier)
    {
        event->accept();
        popup->exec(QCursor::pos());
        return;
    }
    QGLViewer::mousePressEvent(event);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::init()
{
    setFPSIsDisplayed(true);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::clear()
{
    drawlist.clear();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::draw()
{
    setBackgroundColor(clear_color);

    for(auto obj : drawlist)
    {
        obj->draw( sceneRadius() );
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::push_obj(DrawableObject *obj, bool refit_scene)
{
    drawlist.push_back(obj);

    if (refit_scene)
    {
        fit_scene();
    }

    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::fit_scene()
{
    vec3d center(0,0,0);
    float radius = 0.0;
    int   count  = 0;

    for(const DrawableObject *obj : drawlist)
    {
        center += obj->scene_center();
        radius  = std::max(radius, obj->scene_radius());
        ++count;
    }

    center /= (double)count;

    setSceneCenter(qglviewer::Vec(center.x(), center.y(), center.z()));
    setSceneRadius(radius);

    showEntireScene();

    //cout << endl;
    //cout << "Update Scene Center: " << center << endl;
    //cout << "Update Scene Radius: " << radius << endl;
    //cout << endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::set_clear_color(const QColor &color)
{
    clear_color = color;
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
std::string GLcanvas::serialize_drawlist() const
{
    std::stringstream ss;
    for(const DrawableObject *obj : drawlist)
    {
        switch (obj->object_type())
        {
            case DRAWABLE_TRIMESH        : ss << "Trimesh        "; break;
            case DRAWABLE_QUADMESH       : ss << "Quadmesh       "; break;
            case DRAWABLE_POLYGONMESH    : ss << "Polygonmesh    "; break;
            case DRAWABLE_TETMESH        : ss << "Tetmesh        "; break;
            case DRAWABLE_HEXMESH        : ss << "Hexmesh        "; break;
            case DRAWABLE_POLYHEDRALMESH : ss << "Polyhedralmesh "; break;
            case DRAWABLE_CURVE          : ss << "Curve          "; break;
            case DRAWABLE_SKELETON       : ss << "Skeleton       "; break;
            case DRAWABLE_ISOSURFACE     : ss << "Isosurface     "; break;
            case DRAWABLE_VECTOR_FIELD   : ss << "VectorField    "; break;
            default                      : assert(false && "Unknown Drawable Object!");
        }
    }
    return ss.str();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::string GLcanvas::serialize_camera() const
{
    std::stringstream ss;
    ss << this->camera()->position().x      << " "
       << this->camera()->position().y      << " "
       << this->camera()->position().z      << " "
       << this->camera()->orientation()[0]  << " "
       << this->camera()->orientation()[1]  << " "
       << this->camera()->orientation()[2]  << " "
       << this->camera()->orientation()[3]  << " "
       << this->camera()->upVector().x      << " "
       << this->camera()->upVector().y      << " "
       << this->camera()->upVector().z      << " "
       << this->camera()->viewDirection().x << " "
       << this->camera()->viewDirection().y << " "
       << this->camera()->viewDirection().z;
    return ss.str();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::deserialize_camera(const std::string & s)
{
    double pos_x, pos_y, pos_z, or_i, or_j, or_k, or_l, up_x, up_y, up_z, dir_x, dir_y, dir_z;

    std::stringstream ss(s);
    ss >> pos_x >> pos_y >> pos_z
       >> or_i  >> or_j  >> or_k >> or_l
       >> up_x  >> up_y  >> up_z
       >> dir_x >> dir_y >> dir_z;

    this->camera()->setPosition     (qglviewer::Vec(pos_x, pos_y, pos_z));
    this->camera()->setUpVector     (qglviewer::Vec(up_x,up_y,up_z));
    this->camera()->setViewDirection(qglviewer::Vec(dir_x,dir_y,dir_z));
    this->camera()->setOrientation  (qglviewer::Quaternion(or_i,or_j,or_k,or_l));
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool GLcanvas::pop_first_occurrence_of(int type)
{
    for(std::vector<DrawableObject*>::iterator it=drawlist.begin(); it!=drawlist.end(); ++it)
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
bool GLcanvas::pop(DrawableObject *obj)
{
    for(std::vector<DrawableObject*>::iterator it=drawlist.begin(); it!=drawlist.end(); ++it)
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
void GLcanvas::updateGL()
{
    // http://libqglviewer.com/changeLog.html
    #if QGLVIEWER_VERSION < 0x020700
        QGLViewer::updateGL();
    #else
        QGLViewer::update();
    #endif
}

}
