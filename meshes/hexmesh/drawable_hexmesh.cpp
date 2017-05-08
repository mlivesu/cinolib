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
#include <cinolib/meshes/hexmesh/drawable_hexmesh.h>
#include <cinolib/textures/quality_ramp_texture.h>
#include <cinolib/textures/quality_ramp_texture_plus_isolines.h>
#include <cinolib/textures/isolines_texture.h>
#include <cinolib/colors.h>


namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
DrawableHexmesh<M,V,E,F,C>::DrawableHexmesh() : Hexmesh<M,V,E,F,C>()
{
    init_drawable_stuff();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
DrawableHexmesh<M,V,E,F,C>::DrawableHexmesh(const char *filename)
    : Hexmesh<M,V,E,F,C>(filename)
{
    init_drawable_stuff();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
DrawableHexmesh<M,V,E,F,C>::DrawableHexmesh(const std::vector<vec3d> & verts,
                                            const std::vector<uint>  & cells)
    : Hexmesh<M,V,E,F,C>(verts, cells)
{
    init_drawable_stuff();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
DrawableHexmesh<M,V,E,F,C>::DrawableHexmesh(const std::vector<double> & coords,
                                            const std::vector<uint>   & cells)
    : Hexmesh<M,V,E,F,C>(coords, cells)
{
    init_drawable_stuff();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawableHexmesh<M,V,E,F,C>::init_drawable_stuff()
{
    type   = HEXMESH;
    slicer = MeshSlicer<DrawableHexmesh<M,V,E,F,C>>(*this);

    drawlist_in.draw_mode    = DRAW_TRIS | DRAW_TRI_FLAT | DRAW_TRI_FACECOLOR | DRAW_SEGS;
    drawlist_in.seg_width    = 1;
    drawlist_in.seg_color[0] = 0.1;
    drawlist_in.seg_color[1] = 0.1;
    drawlist_in.seg_color[2] = 0.1;
    drawlist_in.seg_color[3] = 1.0;

    drawlist_out.draw_mode    = DRAW_TRIS | DRAW_TRI_FLAT | DRAW_TRI_FACECOLOR | DRAW_SEGS;
    drawlist_out.seg_width    = 1;
    drawlist_out.seg_color[0] = 0.1;
    drawlist_out.seg_color[1] = 0.1;
    drawlist_out.seg_color[2] = 0.1;
    drawlist_out.seg_color[3] = 1.0;

    this->face_set_color(Color::YELLOW());
    this->cell_set_color(Color::WHITE());

    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawableHexmesh<M,V,E,F,C>::draw(const float) const
{
    render(drawlist_in );
    render(drawlist_out);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawableHexmesh<M,V,E,F,C>::updateGL()
{
    updateGL_out();
    updateGL_in();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawableHexmesh<M,V,E,F,C>::updateGL_out()
{
    drawlist_out.tris.clear();
    drawlist_out.tri_coords.clear();
    drawlist_out.tri_v_norms.clear();
    drawlist_out.tri_v_colors.clear();
    drawlist_out.segs.clear();
    drawlist_out.seg_coords.clear();

    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        if (!(this->cell_data(this->adj_f2c(fid)).visible)) continue;

        uint vid0 = this->face_vert_id(fid,0);
        uint vid1 = this->face_vert_id(fid,1);
        uint vid2 = this->face_vert_id(fid,2);
        uint vid3 = this->face_vert_id(fid,3);

        uint base_addr = drawlist_out.tri_coords.size()/3;

        drawlist_out.tris.push_back(base_addr    ); // v0 v1 v2
        drawlist_out.tris.push_back(base_addr + 1);
        drawlist_out.tris.push_back(base_addr + 2);
        drawlist_out.tris.push_back(base_addr    ); // v0 v2 v3
        drawlist_out.tris.push_back(base_addr + 2);
        drawlist_out.tris.push_back(base_addr + 3);

        drawlist_out.tri_coords.push_back(this->vert(vid0).x());
        drawlist_out.tri_coords.push_back(this->vert(vid0).y());
        drawlist_out.tri_coords.push_back(this->vert(vid0).z());
        drawlist_out.tri_coords.push_back(this->vert(vid1).x());
        drawlist_out.tri_coords.push_back(this->vert(vid1).y());
        drawlist_out.tri_coords.push_back(this->vert(vid1).z());
        drawlist_out.tri_coords.push_back(this->vert(vid2).x());
        drawlist_out.tri_coords.push_back(this->vert(vid2).y());
        drawlist_out.tri_coords.push_back(this->vert(vid2).z());
        drawlist_out.tri_coords.push_back(this->vert(vid3).x());
        drawlist_out.tri_coords.push_back(this->vert(vid3).y());
        drawlist_out.tri_coords.push_back(this->vert(vid3).z());

        drawlist_out.tri_v_norms.push_back(this->face_data(fid).normal.x()); // replicate f normal on each vertex
        drawlist_out.tri_v_norms.push_back(this->face_data(fid).normal.y());
        drawlist_out.tri_v_norms.push_back(this->face_data(fid).normal.z());
        drawlist_out.tri_v_norms.push_back(this->face_data(fid).normal.x());
        drawlist_out.tri_v_norms.push_back(this->face_data(fid).normal.y());
        drawlist_out.tri_v_norms.push_back(this->face_data(fid).normal.z());
        drawlist_out.tri_v_norms.push_back(this->face_data(fid).normal.x());
        drawlist_out.tri_v_norms.push_back(this->face_data(fid).normal.y());
        drawlist_out.tri_v_norms.push_back(this->face_data(fid).normal.z());
        drawlist_out.tri_v_norms.push_back(this->face_data(fid).normal.x());
        drawlist_out.tri_v_norms.push_back(this->face_data(fid).normal.y());
        drawlist_out.tri_v_norms.push_back(this->face_data(fid).normal.z());

        if (drawlist_out.draw_mode & DRAW_TRI_FACECOLOR) // replicate f color on each vertex
        {
            drawlist_out.tri_v_colors.push_back(this->face_data(fid).color.r);
            drawlist_out.tri_v_colors.push_back(this->face_data(fid).color.g);
            drawlist_out.tri_v_colors.push_back(this->face_data(fid).color.b);
            drawlist_out.tri_v_colors.push_back(this->face_data(fid).color.a);
            drawlist_out.tri_v_colors.push_back(this->face_data(fid).color.r);
            drawlist_out.tri_v_colors.push_back(this->face_data(fid).color.g);
            drawlist_out.tri_v_colors.push_back(this->face_data(fid).color.b);
            drawlist_out.tri_v_colors.push_back(this->face_data(fid).color.a);
            drawlist_out.tri_v_colors.push_back(this->face_data(fid).color.r);
            drawlist_out.tri_v_colors.push_back(this->face_data(fid).color.g);
            drawlist_out.tri_v_colors.push_back(this->face_data(fid).color.b);
            drawlist_out.tri_v_colors.push_back(this->face_data(fid).color.a);
            drawlist_out.tri_v_colors.push_back(this->face_data(fid).color.r);
            drawlist_out.tri_v_colors.push_back(this->face_data(fid).color.g);
            drawlist_out.tri_v_colors.push_back(this->face_data(fid).color.b);
            drawlist_out.tri_v_colors.push_back(this->face_data(fid).color.a);
        }
        else if (drawlist_out.draw_mode & DRAW_TRI_VERTEXCOLOR)
        {
            drawlist_out.tri_v_colors.push_back(this->vert_data(vid0).color.r);
            drawlist_out.tri_v_colors.push_back(this->vert_data(vid0).color.g);
            drawlist_out.tri_v_colors.push_back(this->vert_data(vid0).color.b);
            drawlist_out.tri_v_colors.push_back(this->vert_data(vid0).color.a);
            drawlist_out.tri_v_colors.push_back(this->vert_data(vid1).color.r);
            drawlist_out.tri_v_colors.push_back(this->vert_data(vid1).color.g);
            drawlist_out.tri_v_colors.push_back(this->vert_data(vid1).color.b);
            drawlist_out.tri_v_colors.push_back(this->vert_data(vid1).color.a);
            drawlist_out.tri_v_colors.push_back(this->vert_data(vid2).color.r);
            drawlist_out.tri_v_colors.push_back(this->vert_data(vid2).color.g);
            drawlist_out.tri_v_colors.push_back(this->vert_data(vid2).color.b);
            drawlist_out.tri_v_colors.push_back(this->vert_data(vid2).color.a);
            drawlist_out.tri_v_colors.push_back(this->vert_data(vid3).color.r);
            drawlist_out.tri_v_colors.push_back(this->vert_data(vid3).color.g);
            drawlist_out.tri_v_colors.push_back(this->vert_data(vid3).color.b);
            drawlist_out.tri_v_colors.push_back(this->vert_data(vid3).color.a);
        }

        // bake wireframe
        base_addr = drawlist_out.seg_coords.size()/3;
        //
        drawlist_out.segs.push_back(base_addr    ); // v0 v1
        drawlist_out.segs.push_back(base_addr + 1);
        drawlist_out.segs.push_back(base_addr + 1); // v1 v2
        drawlist_out.segs.push_back(base_addr + 2);
        drawlist_out.segs.push_back(base_addr + 2); // v2 v3
        drawlist_out.segs.push_back(base_addr + 3);
        drawlist_out.segs.push_back(base_addr + 3); // v3 v0
        drawlist_out.segs.push_back(base_addr    );

        drawlist_out.seg_coords.push_back(this->vert(vid0).x());
        drawlist_out.seg_coords.push_back(this->vert(vid0).y());
        drawlist_out.seg_coords.push_back(this->vert(vid0).z());
        drawlist_out.seg_coords.push_back(this->vert(vid1).x());
        drawlist_out.seg_coords.push_back(this->vert(vid1).y());
        drawlist_out.seg_coords.push_back(this->vert(vid1).z());
        drawlist_out.seg_coords.push_back(this->vert(vid2).x());
        drawlist_out.seg_coords.push_back(this->vert(vid2).y());
        drawlist_out.seg_coords.push_back(this->vert(vid2).z());
        drawlist_out.seg_coords.push_back(this->vert(vid3).x());
        drawlist_out.seg_coords.push_back(this->vert(vid3).y());
        drawlist_out.seg_coords.push_back(this->vert(vid3).z());
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawableHexmesh<M,V,E,F,C>::updateGL_in()
{
    drawlist_in.tri_coords.clear();
    drawlist_in.tris.clear();
    drawlist_in.tri_v_norms.clear();
    drawlist_in.tri_v_colors.clear();
    drawlist_in.segs.clear();
    drawlist_in.seg_coords.clear();

    for(uint cid=0; cid<this->num_cells(); ++cid)
    {
        if (!(this->cell_data(cid).visible)) continue;

        for(uint nbr : this->adj_c2c(cid))
        {
            if (!(this->cell_data(nbr).visible))
            {
                int f = this->cell_shared_face(cid,nbr);
                assert(f!=-1);

                uint vid0 = this->cell_vert_id(cid, HEXA_FACES[f][0]);
                uint vid1 = this->cell_vert_id(cid, HEXA_FACES[f][1]);
                uint vid2 = this->cell_vert_id(cid, HEXA_FACES[f][2]);
                uint vid3 = this->cell_vert_id(cid, HEXA_FACES[f][3]);

                uint base_addr = drawlist_in.tri_coords.size()/3;

                drawlist_in.tris.push_back(base_addr    ); // v0 v1 v2
                drawlist_in.tris.push_back(base_addr + 1);
                drawlist_in.tris.push_back(base_addr + 2);
                drawlist_in.tris.push_back(base_addr    ); // v0 v2 v3
                drawlist_in.tris.push_back(base_addr + 2);
                drawlist_in.tris.push_back(base_addr + 3);

                drawlist_in.tri_coords.push_back(this->vert(vid0).x());
                drawlist_in.tri_coords.push_back(this->vert(vid0).y());
                drawlist_in.tri_coords.push_back(this->vert(vid0).z());
                drawlist_in.tri_coords.push_back(this->vert(vid1).x());
                drawlist_in.tri_coords.push_back(this->vert(vid1).y());
                drawlist_in.tri_coords.push_back(this->vert(vid1).z());
                drawlist_in.tri_coords.push_back(this->vert(vid2).x());
                drawlist_in.tri_coords.push_back(this->vert(vid2).y());
                drawlist_in.tri_coords.push_back(this->vert(vid2).z());
                drawlist_in.tri_coords.push_back(this->vert(vid3).x());
                drawlist_in.tri_coords.push_back(this->vert(vid3).y());
                drawlist_in.tri_coords.push_back(this->vert(vid3).z());

                vec3d v0 = this->vert(vid0);
                vec3d v1 = this->vert(vid1);
                vec3d v2 = this->vert(vid2);
                vec3d n = (v1-v0).cross(v2-v0);
                n.normalize();

                drawlist_in.tri_v_norms.push_back(n.x()); // replicate f normal on each vertex
                drawlist_in.tri_v_norms.push_back(n.y());
                drawlist_in.tri_v_norms.push_back(n.z());
                drawlist_in.tri_v_norms.push_back(n.x());
                drawlist_in.tri_v_norms.push_back(n.y());
                drawlist_in.tri_v_norms.push_back(n.z());
                drawlist_in.tri_v_norms.push_back(n.x());
                drawlist_in.tri_v_norms.push_back(n.y());
                drawlist_in.tri_v_norms.push_back(n.z());
                drawlist_in.tri_v_norms.push_back(n.x());
                drawlist_in.tri_v_norms.push_back(n.y());
                drawlist_in.tri_v_norms.push_back(n.z());

                if (drawlist_in.draw_mode & DRAW_TRI_FACECOLOR) // replicate f color on each vertex
                {
                    drawlist_in.tri_v_colors.push_back(this->cell_data(cid).color.r);
                    drawlist_in.tri_v_colors.push_back(this->cell_data(cid).color.g);
                    drawlist_in.tri_v_colors.push_back(this->cell_data(cid).color.b);
                    drawlist_in.tri_v_colors.push_back(this->cell_data(cid).color.a);
                    drawlist_in.tri_v_colors.push_back(this->cell_data(cid).color.r);
                    drawlist_in.tri_v_colors.push_back(this->cell_data(cid).color.g);
                    drawlist_in.tri_v_colors.push_back(this->cell_data(cid).color.b);
                    drawlist_in.tri_v_colors.push_back(this->cell_data(cid).color.a);
                    drawlist_in.tri_v_colors.push_back(this->cell_data(cid).color.r);
                    drawlist_in.tri_v_colors.push_back(this->cell_data(cid).color.g);
                    drawlist_in.tri_v_colors.push_back(this->cell_data(cid).color.b);
                    drawlist_in.tri_v_colors.push_back(this->cell_data(cid).color.a);
                    drawlist_in.tri_v_colors.push_back(this->cell_data(cid).color.r);
                    drawlist_in.tri_v_colors.push_back(this->cell_data(cid).color.g);
                    drawlist_in.tri_v_colors.push_back(this->cell_data(cid).color.b);
                    drawlist_in.tri_v_colors.push_back(this->cell_data(cid).color.a);
                }
                else if (drawlist_in.draw_mode & DRAW_TRI_VERTEXCOLOR)
                {
                    drawlist_in.tri_v_colors.push_back(this->vert_data(vid0).color.r);
                    drawlist_in.tri_v_colors.push_back(this->vert_data(vid0).color.g);
                    drawlist_in.tri_v_colors.push_back(this->vert_data(vid0).color.b);
                    drawlist_in.tri_v_colors.push_back(this->vert_data(vid0).color.a);
                    drawlist_in.tri_v_colors.push_back(this->vert_data(vid1).color.r);
                    drawlist_in.tri_v_colors.push_back(this->vert_data(vid1).color.g);
                    drawlist_in.tri_v_colors.push_back(this->vert_data(vid1).color.b);
                    drawlist_in.tri_v_colors.push_back(this->vert_data(vid1).color.a);
                    drawlist_in.tri_v_colors.push_back(this->vert_data(vid2).color.r);
                    drawlist_in.tri_v_colors.push_back(this->vert_data(vid2).color.g);
                    drawlist_in.tri_v_colors.push_back(this->vert_data(vid2).color.b);
                    drawlist_in.tri_v_colors.push_back(this->vert_data(vid2).color.a);
                    drawlist_in.tri_v_colors.push_back(this->vert_data(vid3).color.r);
                    drawlist_in.tri_v_colors.push_back(this->vert_data(vid3).color.g);
                    drawlist_in.tri_v_colors.push_back(this->vert_data(vid3).color.b);
                    drawlist_in.tri_v_colors.push_back(this->vert_data(vid3).color.a);
                }

                // bake wireframe
                base_addr = drawlist_in.seg_coords.size()/3;
                //
                drawlist_in.segs.push_back(base_addr    ); // v0 v1
                drawlist_in.segs.push_back(base_addr + 1);
                drawlist_in.segs.push_back(base_addr + 1); // v1 v2
                drawlist_in.segs.push_back(base_addr + 2);
                drawlist_in.segs.push_back(base_addr + 2); // v2 v3
                drawlist_in.segs.push_back(base_addr + 3);
                drawlist_in.segs.push_back(base_addr + 3); // v3 v0
                drawlist_in.segs.push_back(base_addr    );

                drawlist_in.seg_coords.push_back(this->vert(vid0).x());
                drawlist_in.seg_coords.push_back(this->vert(vid0).y());
                drawlist_in.seg_coords.push_back(this->vert(vid0).z());
                drawlist_in.seg_coords.push_back(this->vert(vid1).x());
                drawlist_in.seg_coords.push_back(this->vert(vid1).y());
                drawlist_in.seg_coords.push_back(this->vert(vid1).z());
                drawlist_in.seg_coords.push_back(this->vert(vid2).x());
                drawlist_in.seg_coords.push_back(this->vert(vid2).y());
                drawlist_in.seg_coords.push_back(this->vert(vid2).z());
                drawlist_in.seg_coords.push_back(this->vert(vid3).x());
                drawlist_in.seg_coords.push_back(this->vert(vid3).y());
                drawlist_in.seg_coords.push_back(this->vert(vid3).z());
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawableHexmesh<M,V,E,F,C>::slice(const float thresh, // thresh on centroids or quality
                                       const int   item,   // X, Y, Z, L, Q
                                       const int   sign,   // either LEQ or GEQ
                                       const int   mode)   // either AND or OR
{
    slicer.update(*this, thresh, item, sign, mode); // update per element visibility flags
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

}
