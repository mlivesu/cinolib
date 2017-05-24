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
#include <cinolib/meshes/trimesh/drawable_tri.h>


namespace cinolib
{

template<class M, class V, class E, class F>
CINO_INLINE
DrawableTri<M,V,E,F>::DrawableTri() : Tri<M,V,E,F>()
{
    init_drawable_stuff();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
DrawableTri<M,V,E,F>::DrawableTri(const char * filename) : Tri<M,V,E,F>(filename)
{
    init_drawable_stuff();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
DrawableTri<M,V,E,F>::DrawableTri(const std::vector<vec3d> & verts,
                                  const std::vector<uint>  & faces)
    : Tri<M,V,E,F>(verts,faces)
{
    init_drawable_stuff();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
DrawableTri<M,V,E,F>::DrawableTri(const std::vector<double> & coords,
                                  const std::vector<uint>   & faces)
    : Tri<M,V,E,F>(coords,faces)
{
    init_drawable_stuff();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void DrawableTri<M,V,E,F>::init_drawable_stuff()
{
    type   = TRIMESH;
    slicer = MeshSlicer<Tri<M,V,E,F>>(*this);

    drawlist.draw_mode = DRAW_TRIS | DRAW_TRI_FLAT | DRAW_TRI_FACECOLOR | DRAW_SEGS | DRAW_SEG_SEGCOLOR;
    drawlist.seg_width = 1;

    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void DrawableTri<M,V,E,F>::draw(const float) const
{
    render(drawlist);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void DrawableTri<M,V,E,F>::updateGL()
{
    drawlist.tri_coords.clear();
    drawlist.tris.clear();
    drawlist.tri_v_norms.clear();
    drawlist.tri_v_colors.clear();
    drawlist.segs.clear();
    drawlist.seg_coords.clear();
    drawlist.seg_colors.clear();

    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        if (!(this->face_data(fid).visible)) continue;

        int vid0 = this->face_vert_id(fid,0);
        int vid1 = this->face_vert_id(fid,1);
        int vid2 = this->face_vert_id(fid,2);

        int base_addr = drawlist.tri_coords.size()/3;

        drawlist.tris.push_back(base_addr    );
        drawlist.tris.push_back(base_addr + 1);
        drawlist.tris.push_back(base_addr + 2);

        drawlist.tri_coords.push_back(this->vert(vid0).x());
        drawlist.tri_coords.push_back(this->vert(vid0).y());
        drawlist.tri_coords.push_back(this->vert(vid0).z());
        drawlist.tri_coords.push_back(this->vert(vid1).x());
        drawlist.tri_coords.push_back(this->vert(vid1).y());
        drawlist.tri_coords.push_back(this->vert(vid1).z());
        drawlist.tri_coords.push_back(this->vert(vid2).x());
        drawlist.tri_coords.push_back(this->vert(vid2).y());
        drawlist.tri_coords.push_back(this->vert(vid2).z());

        drawlist.tri_v_norms.push_back(this->vert_data(vid0).normal.x());
        drawlist.tri_v_norms.push_back(this->vert_data(vid0).normal.y());
        drawlist.tri_v_norms.push_back(this->vert_data(vid0).normal.z());
        drawlist.tri_v_norms.push_back(this->vert_data(vid1).normal.x());
        drawlist.tri_v_norms.push_back(this->vert_data(vid1).normal.y());
        drawlist.tri_v_norms.push_back(this->vert_data(vid1).normal.z());
        drawlist.tri_v_norms.push_back(this->vert_data(vid2).normal.x());
        drawlist.tri_v_norms.push_back(this->vert_data(vid2).normal.y());
        drawlist.tri_v_norms.push_back(this->vert_data(vid2).normal.z());

        if (drawlist.draw_mode & DRAW_TRI_FACECOLOR) // replicate f color on each vertex
        {
            drawlist.tri_v_colors.push_back(this->face_data(fid).color.r);
            drawlist.tri_v_colors.push_back(this->face_data(fid).color.g);
            drawlist.tri_v_colors.push_back(this->face_data(fid).color.b);
            drawlist.tri_v_colors.push_back(this->face_data(fid).color.a);
            drawlist.tri_v_colors.push_back(this->face_data(fid).color.r);
            drawlist.tri_v_colors.push_back(this->face_data(fid).color.g);
            drawlist.tri_v_colors.push_back(this->face_data(fid).color.b);
            drawlist.tri_v_colors.push_back(this->face_data(fid).color.a);
            drawlist.tri_v_colors.push_back(this->face_data(fid).color.r);
            drawlist.tri_v_colors.push_back(this->face_data(fid).color.g);
            drawlist.tri_v_colors.push_back(this->face_data(fid).color.b);
            drawlist.tri_v_colors.push_back(this->face_data(fid).color.a);
        }
        else if (drawlist.draw_mode & DRAW_TRI_VERTCOLOR)
        {
            drawlist.tri_v_colors.push_back(this->vert_data(vid0).color.r);
            drawlist.tri_v_colors.push_back(this->vert_data(vid0).color.g);
            drawlist.tri_v_colors.push_back(this->vert_data(vid0).color.b);
            drawlist.tri_v_colors.push_back(this->vert_data(vid0).color.a);
            drawlist.tri_v_colors.push_back(this->vert_data(vid1).color.r);
            drawlist.tri_v_colors.push_back(this->vert_data(vid1).color.g);
            drawlist.tri_v_colors.push_back(this->vert_data(vid1).color.b);
            drawlist.tri_v_colors.push_back(this->vert_data(vid1).color.a);
            drawlist.tri_v_colors.push_back(this->vert_data(vid2).color.r);
            drawlist.tri_v_colors.push_back(this->vert_data(vid2).color.g);
            drawlist.tri_v_colors.push_back(this->vert_data(vid2).color.b);
            drawlist.tri_v_colors.push_back(this->vert_data(vid2).color.a);
        }
    }

    // bake wireframe as border
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        bool masked = true;
        for(uint fid : this->adj_e2f(eid))
        {
            if (this->face_data(fid).visible) masked = false;
        }
        if (masked) continue;

        int base_addr = drawlist.seg_coords.size()/3;
        drawlist.segs.push_back(base_addr    );
        drawlist.segs.push_back(base_addr + 1);

        vec3d vid0 = this->edge_vert(eid,0);
        vec3d vid1 = this->edge_vert(eid,1);

        drawlist.seg_coords.push_back(vid0.x());
        drawlist.seg_coords.push_back(vid0.y());
        drawlist.seg_coords.push_back(vid0.z());
        drawlist.seg_coords.push_back(vid1.x());
        drawlist.seg_coords.push_back(vid1.y());
        drawlist.seg_coords.push_back(vid1.z());

        if (drawlist.draw_mode & DRAW_SEG_SEGCOLOR)
        {
            drawlist.seg_colors.push_back(this->edge_data(eid).color.r);
            drawlist.seg_colors.push_back(this->edge_data(eid).color.g);
            drawlist.seg_colors.push_back(this->edge_data(eid).color.b);
            drawlist.seg_colors.push_back(this->edge_data(eid).color.a);
            drawlist.seg_colors.push_back(this->edge_data(eid).color.r);
            drawlist.seg_colors.push_back(this->edge_data(eid).color.g);
            drawlist.seg_colors.push_back(this->edge_data(eid).color.b);
            drawlist.seg_colors.push_back(this->edge_data(eid).color.a);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void DrawableTri<M,V,E,F>::slice(const float thresh, // thresh on centroids or quality
                                 const int   item,   // X, Y, Z, L, Q
                                 const int   sign,   // either LEQ or GEQ
                                 const int   mode)   // either AND or OR
{
    slicer.update(*this, thresh, item, sign, mode); // update per element visibility flags
    updateGL();
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void DrawableTri<M,V,E,F>::operator+=(const DrawableTri<M,V,E,F> & m)
{
    Tri<M,V,E,F>::operator +=(m);
    updateGL();
}

}
