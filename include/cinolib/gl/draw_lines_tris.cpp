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
#include <cinolib/gl/draw_lines_tris.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void render_tris(const RenderData & data)
{
    if(data.draw_mode & DRAW_TRI_POINTS)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, 0, data.tri_v_colors.data());
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, data.tri_coords.data());
        glPointSize(data.seg_width);
        glDrawArrays(GL_POINTS, 0, (GLsizei)(data.tri_coords.size()/3));
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }
    else
    {
        if(data.draw_mode & DRAW_TRI_TEXTURE1D)
        {
            glBindTexture(GL_TEXTURE_1D, data.texture.id);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(1, GL_FLOAT, 0, data.tri_text.data());
            glColor3f(1,1,1);
            glEnable(GL_COLOR_MATERIAL);
            glEnable(GL_TEXTURE_1D);
        }
        else if(data.draw_mode & DRAW_TRI_TEXTURE2D)
        {
            glBindTexture(GL_TEXTURE_2D, data.texture.id);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, 0, data.tri_text.data());
            glColor3f(1,1,1);
            glEnable(GL_COLOR_MATERIAL);
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
        glDrawElements(GL_TRIANGLES, (GLsizei)data.tris.size(), GL_UNSIGNED_INT, data.tris.data());
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        if(data.draw_mode & DRAW_TRI_TEXTURE1D)
        {
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisable(GL_TEXTURE_1D);
            glEnable(GL_COLOR_MATERIAL);
        }
        else if(data.draw_mode & DRAW_TRI_TEXTURE2D)
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
    if(data.draw_mode & DRAW_SEGS)
    {
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);        
        glDisable(GL_LIGHTING);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, data.seg_coords.data());
        glLineWidth(data.seg_width);
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, 0, data.seg_colors.data());
        glDrawElements(GL_LINES, (GLsizei)data.segs.size(), GL_UNSIGNED_INT, data.segs.data());
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glEnable(GL_LIGHTING);
        glDisable(GL_LINE_SMOOTH);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void render(const RenderData & data)
{
    data.material.apply();

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glPolygonOffset(1.0, 1);

    if(data.draw_mode & DRAW_TRIS)
    {
        if(data.draw_mode & DRAW_TRI_POINTS)
        {
            glDisable(GL_LIGHTING);
            render_tris(data);
        }
        else if(data.draw_mode & DRAW_TRI_SMOOTH)
        {
            glEnable(GL_LIGHTING);
            glShadeModel(GL_SMOOTH);
            render_tris(data);
        }
        else // default: FLAT shading
        {
            glEnable(GL_LIGHTING);
            glShadeModel(GL_SMOOTH); // flatness is given by input normals
            render_tris(data);
        }

        if(data.draw_mode & DRAW_SEGS)
        {
            glDisable(GL_POLYGON_OFFSET_FILL);
            glPushAttrib(GL_POLYGON_BIT);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            render_segs(data);
            glPopAttrib();
            glEnable(GL_POLYGON_OFFSET_FILL);
        }
    }
}

}
