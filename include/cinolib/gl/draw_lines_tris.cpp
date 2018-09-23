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
#include <cinolib/gl/draw_lines_tris.h>

namespace cinolib
{

CINO_INLINE
void render_tris(const RenderData & data)
{
    if (data.draw_mode & DRAW_TRI_POINTS)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, 0, data.tri_v_colors.data());
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, data.tri_coords.data());
        glPointSize(data.seg_width);
        glDrawArrays(GL_POINTS, 0, data.tri_coords.size()/3);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }
    else
    {
        if (data.draw_mode & DRAW_TRI_TEXTURE1D)
        {
            glBindTexture(GL_TEXTURE_1D, data.texture.id);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, data.texture.size, 0, GL_RGB, GL_UNSIGNED_BYTE, data.texture.data);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R,     GL_REPEAT);

            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(1, GL_FLOAT, 0, data.tri_text.data());
            glColor3f(1,1,1);
            glDisable(GL_COLOR_MATERIAL);
            glEnable(GL_TEXTURE_1D);
        }
        else if (data.draw_mode & DRAW_TRI_TEXTURE2D)
        {
            glBindTexture(GL_TEXTURE_2D, data.texture.id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.texture.size, data.texture.size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.texture.data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glGenerateMipmap(GL_TEXTURE_2D);

            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, 0, data.tri_text.data());
            glColor3f(1,1,1);
            glDisable(GL_COLOR_MATERIAL);
            glEnable(GL_TEXTURE_2D);
        }
        else
        {
            glEnable(GL_COLOR_MATERIAL);
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(4, GL_FLOAT, 0, data.tri_v_colors.data());
        }
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, data.tri_coords.data());
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, data.tri_v_norms.data());
        glDrawElements(GL_TRIANGLES, data.tris.size(), GL_UNSIGNED_INT, data.tris.data());
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        if (data.draw_mode & DRAW_TRI_TEXTURE1D)
        {
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisable(GL_TEXTURE_1D);
            glEnable(GL_COLOR_MATERIAL);
        }
        else if (data.draw_mode & DRAW_TRI_TEXTURE2D)
        {
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_COLOR_MATERIAL);
        }
        else
        {
            glDisableClientState(GL_COLOR_ARRAY);
            glDisable(GL_COLOR_MATERIAL);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void render_segs(const RenderData & data)
{
    if (data.draw_mode & DRAW_SEGS)
    {
        glDisable(GL_LIGHTING);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glDepthRange(0.0, 1.0);
        glDepthFunc(GL_LEQUAL);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, data.seg_coords.data());
        glLineWidth(data.seg_width);
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, 0, data.seg_colors.data());
        glDrawElements(GL_LINES, data.segs.size(), GL_UNSIGNED_INT, data.segs.data());
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDepthFunc(GL_LESS);
        glEnable(GL_LIGHTING);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void render(const RenderData & data)
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    if (data.draw_mode & DRAW_TRIS)
    {
        if (data.draw_mode & DRAW_TRI_POINTS)
        {
            glDisable(GL_LIGHTING);
            render_tris(data);
        }
        else if (data.draw_mode & DRAW_TRI_SMOOTH)
        {
            glEnable(GL_LIGHTING);
            glShadeModel(GL_SMOOTH);
            glDepthRange(0.01, 1.0);
            render_tris(data);
        }
        else // default: FLAT shading
        {
            glEnable(GL_LIGHTING);
            glShadeModel(GL_FLAT);
            glDepthRange(0.01, 1.0);
            render_tris(data);
        }
    }

    if (data.draw_mode & DRAW_SEGS)
    {
        render_segs(data);
    }
}

}
