/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2017                                                         *
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
#include <cinolib/gl/draw_lines_tris.h>

namespace cinolib
{

CINO_INLINE
void render_pvt(const RenderData & data)
{

    if (data.draw_mode & DRAW_TRI_POINTS)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_DOUBLE, 0, data.tri_coords.data());
        glPointSize(data.seg_width);
        glDrawArrays(GL_POINTS, 0, data.tri_coords.size()/3);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
    else if (data.draw_mode & DRAW_TRI_SMOOTH || data.draw_mode & DRAW_TRI_FLAT)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_DOUBLE, 0, data.tri_coords.data());
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_DOUBLE, 0, data.tri_v_norms.data());
        if (data.draw_mode & DRAW_TRI_VERTCOLOR ||
            data.draw_mode & DRAW_TRI_FACECOLOR ||
            data.draw_mode & DRAW_TRI_QUALITY)
        {
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(4, GL_FLOAT, 0, data.tri_v_colors.data());
        }
        else if (data.draw_mode & DRAW_TRI_TEXTURE1D)
        {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(1, GL_FLOAT, 0, data.tri_text1D.data());
            glColor3f(1.0,1.0,1.0);
        }
        glDrawElements(GL_TRIANGLES, data.tris.size(), GL_UNSIGNED_INT, data.tris.data());
        if (data.draw_mode & DRAW_TRI_VERTCOLOR) glDisableClientState(GL_COLOR_ARRAY);   else
        if (data.draw_mode & DRAW_TRI_FACECOLOR) glDisableClientState(GL_COLOR_ARRAY);   else
        if (data.draw_mode & DRAW_TRI_QUALITY)   glDisableClientState(GL_COLOR_ARRAY);   else
        if (data.draw_mode & DRAW_TRI_TEXTURE1D) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (data.draw_mode & DRAW_SEGS)
    {
        glDisable(GL_LIGHTING);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glDepthRange(0.0, 1.0);
        glDepthFunc(GL_LEQUAL);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_DOUBLE, 0, data.seg_coords.data());
        glLineWidth(data.seg_width);
        if (data.draw_mode & DRAW_SEG_SEGCOLOR || data.draw_mode & DRAW_SEG_VERTCOLOR)
        {
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(4, GL_FLOAT, 0, data.seg_colors.data());
        }
        glDrawElements(GL_LINES, data.segs.size(), GL_UNSIGNED_INT, data.segs.data());
        if (data.draw_mode & DRAW_SEG_VERTCOLOR) glDisableClientState(GL_COLOR_ARRAY); else
        if (data.draw_mode & DRAW_SEG_SEGCOLOR)  glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDepthFunc(GL_LESS);
        glEnable(GL_LIGHTING);
    }
}


CINO_INLINE
void render(const RenderData & data)
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glDisable(GL_CULL_FACE);

    if (data.draw_mode & DRAW_TRIS)
    {
        if (data.draw_mode & DRAW_TRI_POINTS)
        {
            glDisable(GL_LIGHTING);
            render_pvt(data);
        }
        else if (data.draw_mode & DRAW_TRI_FLAT)
        {
            glEnable(GL_LIGHTING);
            glShadeModel(GL_FLAT);
            glDepthRange(0.01, 1.0);
            render_pvt(data);
        }
        else if (data.draw_mode & DRAW_TRI_SMOOTH)
        {
            glEnable(GL_LIGHTING);
            glShadeModel(GL_SMOOTH);
            glDepthRange(0.01, 1.0);
            render_pvt(data);
        }
    }
}

}
