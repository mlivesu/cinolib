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
#include "draw_tris_quads.h"

namespace cinolib
{

CINO_INLINE
void render_faces_pvt(const RenderFaceData & data)
{
    if (data.draw_mode & DRAW_POINTS)
    {

        assert(data.coords != NULL);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_DOUBLE, 0, data.coords->data());

        assert(data.wireframe_color != NULL);
        glPointSize(data.wireframe_width);
        glColor4fv(data.wireframe_color);

        glDrawArrays(GL_POINTS, 0, data.coords->size()/3);

        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
    else if (data.draw_mode & DRAW_SMOOTH || data.draw_mode & DRAW_FLAT)
    {
        // Old fashioned (orrible) rendering
        //
        if (data.draw_mode & DRAW_FACECOLOR)
        {
            assert(data.faces    != NULL);
            assert(data.v_norms  != NULL);
            assert(data.f_colors != NULL);
            assert(data.coords   != NULL);

            bool quads   = (data.face_type == GL_QUADS);
            int  n_faces = (quads) ? data.faces->size()/4 : data.faces->size()/3;

            for(int fid=0; fid<n_faces; ++fid)
            {
                int fid_ptr  = (quads) ? 4*fid : 3*fid;
                int vid0     = data.faces->at(fid_ptr + 0);
                int vid1     = data.faces->at(fid_ptr + 1);
                int vid2     = data.faces->at(fid_ptr + 2);
                int vid3     = (quads) ? data.faces->at(fid_ptr + 3) : -1;
                int vid0_ptr = 3 * vid0;
                int vid1_ptr = 3 * vid1;
                int vid2_ptr = 3 * vid2;
                int vid3_ptr = 3 * vid3;

                glBegin(data.face_type);
                glColor3fv (&(data.f_colors->at(3*fid)));
                glNormal3dv(&(data.v_norms->at(vid0_ptr)));
                glVertex3dv(&(data.coords->at(vid0_ptr)));
                glNormal3dv(&(data.v_norms->at(vid1_ptr)));
                glVertex3dv(&(data.coords->at(vid1_ptr)));
                glNormal3dv(&(data.v_norms->at(vid2_ptr)));
                glVertex3dv(&(data.coords->at(vid2_ptr)));
                if (quads)
                {
                    glNormal3dv(&(data.v_norms->at(vid3_ptr)));
                    glVertex3dv(&(data.coords->at(vid3_ptr)));
                }
                glEnd();
            }
        }
        else
        {
            assert(data.coords != NULL);
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_DOUBLE, 0, data.coords->data());

            assert(data.v_norms != NULL);
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_DOUBLE, 0, data.v_norms->data());

            if (data.draw_mode & DRAW_VERTEXCOLOR)
            {
                assert(data.v_colors != NULL);
                glEnableClientState(GL_COLOR_ARRAY);
                glColorPointer(3, GL_FLOAT, 0, data.v_colors->data());
            }
            else if (data.draw_mode & DRAW_TEXTURE1D)
            {
                assert(data.text1D != NULL);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexCoordPointer(1, GL_FLOAT, 0, data.text1D->data());
                glColor3f(1.0,1.0,1.0);
            }

            assert(data.faces != NULL);
            glDrawElements(data.face_type, data.faces->size(), GL_UNSIGNED_INT, data.faces->data());

            if (data.draw_mode & DRAW_VERTEXCOLOR) glDisableClientState(GL_COLOR_ARRAY);   else
            if (data.draw_mode & DRAW_TEXTURE1D)   glDisableClientState(GL_TEXTURE_COORD_ARRAY);


            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
        }
    }

    if (data.draw_mode & DRAW_WIREFRAME)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        assert(data.coords != NULL);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_DOUBLE, 0, data.coords->data());

        assert(data.wireframe_color != NULL);
        glLineWidth(data.wireframe_width);
        glColor4fv(data.wireframe_color);

        assert(data.faces != NULL);
        glDrawElements(data.face_type, data.faces->size(), GL_UNSIGNED_INT, data.faces->data());

        glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (data.draw_mode & DRAW_BORDER)
    {
        glDisable(GL_LIGHTING);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        assert(data.border_coords != NULL);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_DOUBLE, 0, data.border_coords->data());

        assert(data.border_color != NULL);
        glLineWidth(data.border_width);
        glColor4fv(data.border_color);

        assert(data.border_segs != NULL);
        glDrawElements(GL_LINES, data.border_segs->size(), GL_UNSIGNED_INT, data.border_segs->data());

        glDisableClientState(GL_VERTEX_ARRAY);
    }
}


CINO_INLINE
void render_faces(const RenderFaceData & data)
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glDisable(GL_CULL_FACE);

    if (data.draw_mode & DRAW_MESH)
    {
        if (data.draw_mode & DRAW_WIREFRAME)
        {
            if (data.draw_mode & DRAW_POINTS)
            {
                glDisable(GL_LIGHTING);
                glShadeModel(GL_FLAT);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDepthRange(0.0, 1.0);
                render_faces_pvt(data);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            else if (data.draw_mode & DRAW_FLAT)
            {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_FLAT);
                glDepthRange(0.01, 1.0);
                render_faces_pvt(data);

                glDisable(GL_LIGHTING);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDepthRange(0.0, 1.0);
                glDepthFunc(GL_LEQUAL);
                render_faces_pvt(data);
                glDepthFunc(GL_LESS);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            else if (data.draw_mode & DRAW_SMOOTH)
            {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_SMOOTH);
                glDepthRange(0.01, 1.0);
                render_faces_pvt(data);

                glDisable(GL_LIGHTING);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDepthRange(0.0, 1.0);
                glDepthFunc(GL_LEQUAL);
                render_faces_pvt(data);
                glDepthFunc(GL_LESS);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
        else // no wireframe
        {
            if (data.draw_mode & DRAW_POINTS)
            {
                glDisable(GL_LIGHTING);
                render_faces_pvt(data);
            }
            else if (data.draw_mode & DRAW_FLAT)
            {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_FLAT);
                render_faces_pvt(data);
            }
            else if (data.draw_mode & DRAW_SMOOTH)
            {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_SMOOTH);
                render_faces_pvt(data);
            }
        }
    }
}

}
