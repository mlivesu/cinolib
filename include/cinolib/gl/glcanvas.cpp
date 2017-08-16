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
#include <cinolib/gl/glcanvas.h>

namespace cinolib
{

CINO_INLINE
GLcanvas::GLcanvas(QWidget * parent)
{
    setParent(parent);

    clear_color = QColor(200, 200, 200);
}

CINO_INLINE
void GLcanvas::init()
{
    setFPSIsDisplayed(true);
}

CINO_INLINE
void GLcanvas::clear()
{
    drawlist.clear();
}

CINO_INLINE
void GLcanvas::draw()
{
    setBackgroundColor(clear_color);

    for(auto obj : drawlist)
    {
        obj->draw( sceneRadius() );
    }
}

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

CINO_INLINE
void GLcanvas::set_clear_color(const QColor &color)
{
    clear_color = color;
    updateGL();
}


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

CINO_INLINE
void GLcanvas::set_slice(float thresh, int item, int sign, int mode)
{
    for(DrawableObject *obj : drawlist) obj->slice(thresh, item, sign, mode);
    updateGL();
}

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

}
