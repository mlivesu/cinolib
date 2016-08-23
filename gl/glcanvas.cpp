/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#include <cinolib/gl/glcanvas.h>

namespace cinolib
{

CINO_INLINE GLcanvas::GLcanvas(QWidget * parent)
{
    setParent(parent);

    clear_color = QColor(50, 50, 50);
}

CINO_INLINE void GLcanvas::init()
{
    setFPSIsDisplayed(true);
}

CINO_INLINE void GLcanvas::clear()
{
    drawlist.clear();
}

CINO_INLINE void GLcanvas::draw()
{
    setBackgroundColor(clear_color);

    for(int i=0; i<(int)drawlist.size(); ++i)
    {
        drawlist[i]->draw( sceneRadius() );
    }
}

CINO_INLINE void GLcanvas::push_obj(const DrawableObject * obj, bool refit_scene)
{
    drawlist.push_back(obj);

    if (refit_scene)
    {
        fit_scene();
    }

    updateGL();
}

CINO_INLINE void GLcanvas::fit_scene()
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

CINO_INLINE void GLcanvas::set_clear_color(const QColor &color)
{
    clear_color = color;
    updateGL();
}


CINO_INLINE bool GLcanvas::pop_all_occurrences_of(int type)
{
    bool found = false;

    while (pop_first_occurrence_of(type))
    {
        found = true;
    };

    return found;
}

CINO_INLINE bool GLcanvas::pop_first_occurrence_of(int type)
{
    for(std::vector<const DrawableObject *>::iterator it=drawlist.begin(); it!=drawlist.end(); ++it)
    {
        const DrawableObject * obj = *it;

        if (obj->type == type)
        {
            drawlist.erase(it);
            return true;
        }
    }
    return false;
}

}
