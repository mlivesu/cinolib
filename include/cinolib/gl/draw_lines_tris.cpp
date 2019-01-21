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

///*
// * look at the GOOD SETTINGS section in
// * https://www.khronos.org/opengl/wiki/How_lighting_works
//*/
//static void initLighting()
//{
//    GLfloat light_position0[4] = {0, 0, 50, 0};
//    GLfloat light_position1[4] = {52, 16, 50, 0};
//    GLfloat light_position2[4] = {26, 48, 50, 0};
//    GLfloat light_position3[4] = {-16, 52, 50, 0};
//    GLfloat direction1[3] = {-52,-16,-50};
//    GLfloat direction2[3] = {-26,-48,-50};
//    GLfloat direction3[3] = {16,-52,-50};
//    GLfloat color1[4] = {1,0, 0, 1};
//    GLfloat color2[4] = {0, 1, 0, 1};
//    GLfloat color3[4] = {0, 0, 1, 1};
//    GLfloat ambient[4] = {0.1f, 0.1f, 0.1f, 0.5f};
//    glLightfv (GL_LIGHT0, GL_POSITION, light_position0);
//    glLightfv (GL_LIGHT1, GL_POSITION, light_position1);
//    glLightfv (GL_LIGHT1, GL_SPOT_DIRECTION, direction1);
//    glLightfv (GL_LIGHT1, GL_DIFFUSE, color1);
//    glLightfv (GL_LIGHT1, GL_SPECULAR, color1);
//    glLightfv (GL_LIGHT2, GL_POSITION, light_position2);
//    glLightfv (GL_LIGHT2, GL_SPOT_DIRECTION, direction2);
//    glLightfv (GL_LIGHT2, GL_DIFFUSE, color2);
//    glLightfv (GL_LIGHT2, GL_SPECULAR, color2);
//    glLightfv (GL_LIGHT3, GL_POSITION, light_position3);
//    glLightfv (GL_LIGHT3, GL_SPOT_DIRECTION, direction3);
//    glLightfv (GL_LIGHT3, GL_DIFFUSE, color3);
//    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
//}

//static void initMaterial()
//{
//    GLfloat material_color[4] = {1,1,1,1.0f};
//    GLfloat material_specular[4] = {0.85,0.85,0.85,1.0};
//    GLfloat material_ambient[4] = {0.0,0.0,0.0,0.0};
//    glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
//    glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, material_color);
//    glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, material_ambient);
//    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 128.0f);
//}

//static void initInverseMaterial()
//{
//    GLfloat material_color[4] = {0.0, 0.0, 0.0, 1.0f};
//    GLfloat material_specular[4] = {0.0,0.0,0.0,1.0};
//    GLfloat material_ambient[4] = {0.0,0.0,0.0,0.0};
//    glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
//    glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, material_color);
//    glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, material_ambient);
//    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 0);
//}

//static void setSingleLighting()
//{
//    glEnable (GL_LIGHT0);
//    glDisable (GL_LIGHT1);
//    glDisable (GL_LIGHT2);
//    glDisable (GL_LIGHT3);
//}

//static void setMultiLighting()
//{
//    glDisable (GL_LIGHT0);
//    glEnable (GL_LIGHT1);
//    glEnable (GL_LIGHT2);
//    glEnable (GL_LIGHT3);
//}


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
//    initLighting();
//    setMultiLighting();
//    initMaterial();
//    initInverseMaterial();

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
            glShadeModel(GL_SMOOTH); // flatness is given by input normals
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
