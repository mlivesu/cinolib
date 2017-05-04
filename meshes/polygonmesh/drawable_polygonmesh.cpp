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
#include <cinolib/meshes/polygonmesh/drawable_polygonmesh.h>


namespace cinolib
{

template<class V_data, class E_data, class F_data>
CINO_INLINE
DrawablePolygonmesh<V_data,E_data,F_data>::DrawablePolygonmesh() : Polygonmesh<V_data,E_data,F_data>()
{
    init_drawable_stuff();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class V_data, class E_data, class F_data>
CINO_INLINE
DrawablePolygonmesh<V_data,E_data,F_data>::DrawablePolygonmesh(const std::vector<double>            & coords,
                                                               const std::vector<std::vector<uint>> & faces)
    : Polygonmesh<V_data,E_data,F_data>(coords,faces)
{
    init_drawable_stuff();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class V_data, class E_data, class F_data>
CINO_INLINE
void DrawablePolygonmesh<V_data,E_data,F_data>::init_drawable_stuff()
{
    type = POLYGONMESH;

    draw_mode = DRAW_MESH | DRAW_FLAT | DRAW_FACECOLOR | DRAW_BORDER;

    wireframe_width    = 1;
    wireframe_color[0] = 0.1;
    wireframe_color[1] = 0.1;
    wireframe_color[2] = 0.1;
    wireframe_color[3] = 1.0;

    border_width    = 4;
    border_color[0] = 0.1;
    border_color[1] = 0.1;
    border_color[2] = 0.1;
    border_color[3] = 1.0;

    slicer = MeshSlicer<Polygonmesh<V_data,E_data,F_data>>(this);

    update_drawlist();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class V_data, class E_data, class F_data>
CINO_INLINE
void DrawablePolygonmesh<V_data,E_data,F_data>::draw(const float) const
{
    RenderFaceData data;
    data.face_type       = GL_TRIANGLES;
    data.draw_mode       = draw_mode;
    data.coords          = &drawlist_coords;
    data.faces           = &drawlist_faces;
    data.v_norms         = &drawlist_v_norms;
    data.v_colors        = &drawlist_v_colors;
    data.f_colors        = &drawlist_f_colors;
    data.border_coords   = &drawlist_border_coords;
    data.border_segs     = &drawlist_border_segs;
    data.border_color    = border_color;
    data.border_width    = border_width;
    data.wireframe_color = wireframe_color;
    data.wireframe_width = wireframe_width;

    render_faces(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class V_data, class E_data, class F_data>
CINO_INLINE
void DrawablePolygonmesh<V_data,E_data,F_data>::update_drawlist()
{
    drawlist_coords.clear();
    drawlist_faces.clear();
    drawlist_v_norms.clear();
    drawlist_v_colors.clear();
    drawlist_f_colors.clear();
    drawlist_border_coords.clear();
    drawlist_border_segs.clear();

    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        if ((draw_mode & DRAW_MESH) && !(this->face_data(fid).visible)) continue;

        uint n_sides = this->faces.at(fid).size();
        assert(n_sides>2);

        // trivial triangulation for convex polygon
        for (uint i=2; i<this->faces.at(fid).size(); ++i)
        {
            int vid0     = this->faces.at(fid).at( 0 );
            int vid1     = this->faces.at(fid).at(i-1);
            int vid2     = this->faces.at(fid).at( i );
            int vid0_ptr = 3 * vid0;
            int vid1_ptr = 3 * vid1;
            int vid2_ptr = 3 * vid2;

            int base_addr = drawlist_coords.size()/3;

            drawlist_faces.push_back(base_addr    );
            drawlist_faces.push_back(base_addr + 1);
            drawlist_faces.push_back(base_addr + 2);

            drawlist_coords.push_back(this->coords[vid0_ptr + 0]);
            drawlist_coords.push_back(this->coords[vid0_ptr + 1]);
            drawlist_coords.push_back(this->coords[vid0_ptr + 2]);
            drawlist_coords.push_back(this->coords[vid1_ptr + 0]);
            drawlist_coords.push_back(this->coords[vid1_ptr + 1]);
            drawlist_coords.push_back(this->coords[vid1_ptr + 2]);
            drawlist_coords.push_back(this->coords[vid2_ptr + 0]);
            drawlist_coords.push_back(this->coords[vid2_ptr + 1]);
            drawlist_coords.push_back(this->coords[vid2_ptr + 2]);

            drawlist_v_norms.push_back(this->vert_data(vid0).normal.x());
            drawlist_v_norms.push_back(this->vert_data(vid0).normal.y());
            drawlist_v_norms.push_back(this->vert_data(vid0).normal.z());
            drawlist_v_norms.push_back(this->vert_data(vid1).normal.x());
            drawlist_v_norms.push_back(this->vert_data(vid1).normal.y());
            drawlist_v_norms.push_back(this->vert_data(vid1).normal.z());
            drawlist_v_norms.push_back(this->vert_data(vid2).normal.x());
            drawlist_v_norms.push_back(this->vert_data(vid2).normal.y());
            drawlist_v_norms.push_back(this->vert_data(vid2).normal.z());

            drawlist_v_colors.push_back(this->vert_data(vid0).color[0]);
            drawlist_v_colors.push_back(this->vert_data(vid0).color[1]);
            drawlist_v_colors.push_back(this->vert_data(vid0).color[2]);
            drawlist_v_colors.push_back(this->vert_data(vid1).color[0]);
            drawlist_v_colors.push_back(this->vert_data(vid1).color[1]);
            drawlist_v_colors.push_back(this->vert_data(vid1).color[2]);
            drawlist_v_colors.push_back(this->vert_data(vid2).color[0]);
            drawlist_v_colors.push_back(this->vert_data(vid2).color[1]);
            drawlist_v_colors.push_back(this->vert_data(vid2).color[2]);

            drawlist_f_colors.push_back(this->face_data(fid).color[0]);
            drawlist_f_colors.push_back(this->face_data(fid).color[1]);
            drawlist_f_colors.push_back(this->face_data(fid).color[2]);
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

        int base_addr = drawlist_border_coords.size()/3;
        drawlist_border_segs.push_back(base_addr    );
        drawlist_border_segs.push_back(base_addr + 1);

        vec3d vid0 = this->edge_vert(eid,0);
        vec3d vid1 = this->edge_vert(eid,1);

        drawlist_border_coords.push_back(vid0.x());
        drawlist_border_coords.push_back(vid0.y());
        drawlist_border_coords.push_back(vid0.z());
        drawlist_border_coords.push_back(vid1.x());
        drawlist_border_coords.push_back(vid1.y());
        drawlist_border_coords.push_back(vid1.z());
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class V_data, class E_data, class F_data>
CINO_INLINE
void DrawablePolygonmesh<V_data,E_data,F_data>::slice(const float thresh, // thresh on centroids or quality
                                                      const int   item,   // X, Y, Z, L, Q
                                                      const int   sign,   // either LEQ or GEQ
                                                      const int   mode)   // either AND or OR
{
    slicer.update(thresh, item, sign, mode); // update per element visibility flags
    update_drawlist();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

}
