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
#include <cinolib/meshes/polyhedralmesh/drawable_polyhedralmesh.h>
#include <cinolib/textures/textures.h>
#include <cinolib/colors.h>


namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
DrawablePolyhedralmesh<M,V,E,F,C>::DrawablePolyhedralmesh() : Polyhedralmesh<M,V,E,F,C>()
{
    init_drawable_stuff();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
DrawablePolyhedralmesh<M,V,E,F,C>::DrawablePolyhedralmesh(const std::vector<vec3d>             & verts,
                                                          const std::vector<std::vector<uint>> & faces,
                                                          const std::vector<std::vector<int>>  & cells)
    : Polyhedralmesh<M,V,E,F,C>(verts, faces, cells)
{
    init_drawable_stuff();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::init_drawable_stuff()
{
    type   = POLYHEDRALMESH;
    slicer = MeshSlicer<DrawablePolyhedralmesh<M,V,E,F,C>>(*this);

    drawlist_in.draw_mode = DRAW_TRIS | DRAW_TRI_FLAT | DRAW_TRI_FACECOLOR | DRAW_SEGS | DRAW_SEG_SEGCOLOR;
    drawlist_in.seg_width = 1;

    drawlist_out.draw_mode = DRAW_TRIS | DRAW_TRI_FLAT | DRAW_TRI_FACECOLOR | DRAW_SEGS | DRAW_SEG_SEGCOLOR;
    drawlist_out.seg_width = 1;

//    Polyhedralmesh<M,V,E,F,C>::face_set_color(Color::YELLOW());
//    Polyhedralmesh<M,V,E,F,C>::cell_set_color(Color::WHITE());

    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::draw(const float) const
{
    render(drawlist_in );
    render(drawlist_out);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::updateGL()
{
    updateGL_out();
    updateGL_in();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::updateGL_out()
{
    drawlist_out.tris.clear();
    drawlist_out.tri_coords.clear();
    drawlist_out.tri_v_norms.clear();
    drawlist_out.tri_v_colors.clear();
    drawlist_out.tri_text1D.clear();
    drawlist_out.segs.clear();
    drawlist_out.seg_coords.clear();
    drawlist_out.seg_colors.clear();

    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        if (!this->face_is_on_srf(fid)) continue;

        assert(this->adj_f2c(fid).size()==1);
        uint cid = this->adj_f2c(fid).front();

        if (!(this->cell_data(cid).visible)) continue;

        for(uint i=0; i<this->tessellated_faces.at(fid).size()/3; ++i)
        {
            uint vid0 = this->tessellated_faces.at(fid).at(3*i+0);
            uint vid1 = this->tessellated_faces.at(fid).at(3*i+1);
            uint vid2 = this->tessellated_faces.at(fid).at(3*i+2);

            int base_addr = drawlist_out.tri_coords.size()/3;

            drawlist_out.tris.push_back(base_addr    );
            drawlist_out.tris.push_back(base_addr + 1);
            drawlist_out.tris.push_back(base_addr + 2);

            drawlist_out.tri_coords.push_back(this->vert(vid0).x());
            drawlist_out.tri_coords.push_back(this->vert(vid0).y());
            drawlist_out.tri_coords.push_back(this->vert(vid0).z());
            drawlist_out.tri_coords.push_back(this->vert(vid1).x());
            drawlist_out.tri_coords.push_back(this->vert(vid1).y());
            drawlist_out.tri_coords.push_back(this->vert(vid1).z());
            drawlist_out.tri_coords.push_back(this->vert(vid2).x());
            drawlist_out.tri_coords.push_back(this->vert(vid2).y());
            drawlist_out.tri_coords.push_back(this->vert(vid2).z());

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
                drawlist_out.tri_v_colors.push_back(this->cell_data(cid).color.r);
                drawlist_out.tri_v_colors.push_back(this->cell_data(cid).color.g);
                drawlist_out.tri_v_colors.push_back(this->cell_data(cid).color.b);
                drawlist_out.tri_v_colors.push_back(this->cell_data(cid).color.a);
                drawlist_out.tri_v_colors.push_back(this->cell_data(cid).color.r);
                drawlist_out.tri_v_colors.push_back(this->cell_data(cid).color.g);
                drawlist_out.tri_v_colors.push_back(this->cell_data(cid).color.b);
                drawlist_out.tri_v_colors.push_back(this->cell_data(cid).color.a);
                drawlist_out.tri_v_colors.push_back(this->cell_data(cid).color.r);
                drawlist_out.tri_v_colors.push_back(this->cell_data(cid).color.g);
                drawlist_out.tri_v_colors.push_back(this->cell_data(cid).color.b);
                drawlist_out.tri_v_colors.push_back(this->cell_data(cid).color.a);
            }
            else if (drawlist_out.draw_mode & DRAW_TRI_VERTCOLOR)
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
            }
        }
    }

    // bake wireframe as border
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if (!this->edge_is_on_srf(eid)) continue;

        bool masked = true;
        for(uint cid : this->adj_e2c(eid))
        {
            if (this->cell_data(cid).visible) masked = false;
        }
        if (masked) continue;

        int base_addr = drawlist_out.seg_coords.size()/3;
        drawlist_out.segs.push_back(base_addr    );
        drawlist_out.segs.push_back(base_addr + 1);

        vec3d vid0 = this->edge_vert(eid,0);
        vec3d vid1 = this->edge_vert(eid,1);

        drawlist_out.seg_coords.push_back(vid0.x());
        drawlist_out.seg_coords.push_back(vid0.y());
        drawlist_out.seg_coords.push_back(vid0.z());
        drawlist_out.seg_coords.push_back(vid1.x());
        drawlist_out.seg_coords.push_back(vid1.y());
        drawlist_out.seg_coords.push_back(vid1.z());

        if (drawlist_out.draw_mode & DRAW_SEG_SEGCOLOR)
        {
            drawlist_out.seg_colors.push_back(this->edge_data(eid).color.r);
            drawlist_out.seg_colors.push_back(this->edge_data(eid).color.g);
            drawlist_out.seg_colors.push_back(this->edge_data(eid).color.b);
            drawlist_out.seg_colors.push_back(this->edge_data(eid).color.a);
            drawlist_out.seg_colors.push_back(this->edge_data(eid).color.r);
            drawlist_out.seg_colors.push_back(this->edge_data(eid).color.g);
            drawlist_out.seg_colors.push_back(this->edge_data(eid).color.b);
            drawlist_out.seg_colors.push_back(this->edge_data(eid).color.a);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::updateGL_in()
{
    drawlist_in.tris.clear();
    drawlist_in.tri_coords.clear();
    drawlist_in.tri_v_norms.clear();
    drawlist_in.tri_v_colors.clear();
    drawlist_in.tri_text1D.clear();
    drawlist_in.segs.clear();
    drawlist_in.seg_coords.clear();
    drawlist_in.seg_colors.clear();

    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        if (this->face_is_on_srf(fid)) continue;

        assert(this->adj_f2c(fid).size()==2);
        std::vector<uint> visible_cells;
        for(uint cid : this->adj_f2c(fid))
        {
            if (this->cell_data(cid).visible) visible_cells.push_back(cid);
        }
        if (visible_cells.size()!=1) continue;

        uint cid   = visible_cells.front();
        bool is_CW = this->cell_face_is_CW(cid,this->cell_face_offset(cid,fid));

        for(uint i=0; i<this->tessellated_faces.at(fid).size()/3; ++i)
        {
            uint vid0 = this->tessellated_faces.at(fid).at(3*i+0);
            uint vid1 = this->tessellated_faces.at(fid).at(3*i+1);
            uint vid2 = this->tessellated_faces.at(fid).at(3*i+2);
            if (is_CW) std::swap(vid1,vid2); // flip triangle orientation

            int base_addr = drawlist_in.tri_coords.size()/3;

            drawlist_in.tris.push_back(base_addr    );
            drawlist_in.tris.push_back(base_addr + 1);
            drawlist_in.tris.push_back(base_addr + 2);

            drawlist_in.tri_coords.push_back(this->vert(vid0).x());
            drawlist_in.tri_coords.push_back(this->vert(vid0).y());
            drawlist_in.tri_coords.push_back(this->vert(vid0).z());
            drawlist_in.tri_coords.push_back(this->vert(vid1).x());
            drawlist_in.tri_coords.push_back(this->vert(vid1).y());
            drawlist_in.tri_coords.push_back(this->vert(vid1).z());
            drawlist_in.tri_coords.push_back(this->vert(vid2).x());
            drawlist_in.tri_coords.push_back(this->vert(vid2).y());
            drawlist_in.tri_coords.push_back(this->vert(vid2).z());

            vec3d n = (is_CW) ? -this->face_data(fid).normal : this->face_data(fid).normal;
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
            }
            else if (drawlist_in.draw_mode & DRAW_TRI_VERTCOLOR)
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
            }
        }
    }

    // bake wireframe as border
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if (this->edge_is_on_srf(eid)) continue;

        bool masked = true;
        for(uint cid : this->adj_e2c(eid))
        {
            if (this->cell_data(cid).visible) masked = false;
        }
        if (masked) continue;

        int base_addr = drawlist_in.seg_coords.size()/3;
        drawlist_in.segs.push_back(base_addr    );
        drawlist_in.segs.push_back(base_addr + 1);

        vec3d vid0 = this->edge_vert(eid,0);
        vec3d vid1 = this->edge_vert(eid,1);

        drawlist_in.seg_coords.push_back(vid0.x());
        drawlist_in.seg_coords.push_back(vid0.y());
        drawlist_in.seg_coords.push_back(vid0.z());
        drawlist_in.seg_coords.push_back(vid1.x());
        drawlist_in.seg_coords.push_back(vid1.y());
        drawlist_in.seg_coords.push_back(vid1.z());

        if (drawlist_in.draw_mode & DRAW_SEG_SEGCOLOR)
        {
            drawlist_in.seg_colors.push_back(this->edge_data(eid).color.r);
            drawlist_in.seg_colors.push_back(this->edge_data(eid).color.g);
            drawlist_in.seg_colors.push_back(this->edge_data(eid).color.b);
            drawlist_in.seg_colors.push_back(this->edge_data(eid).color.a);
            drawlist_in.seg_colors.push_back(this->edge_data(eid).color.r);
            drawlist_in.seg_colors.push_back(this->edge_data(eid).color.g);
            drawlist_in.seg_colors.push_back(this->edge_data(eid).color.b);
            drawlist_in.seg_colors.push_back(this->edge_data(eid).color.a);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::slice(const float thresh, // thresh on centroids or quality
                                              const int   item,   // X, Y, Z, L, Q
                                              const int   sign,   // either LEQ or GEQ
                                              const int   mode)   // either AND or OR
{
    slicer.update(*this, thresh, item, sign, mode); // update per element visibility flags
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_mesh(const bool b)
{
    if (b)
    {
        drawlist_in.draw_mode  |= DRAW_TRIS;
        drawlist_out.draw_mode |= DRAW_TRIS;
    }
    else
    {
        drawlist_in.draw_mode  &= ~DRAW_TRIS;
        drawlist_out.draw_mode &= ~DRAW_TRIS;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_mesh_flat()
{
    drawlist_in.draw_mode  |=  DRAW_TRI_FLAT;
    drawlist_in.draw_mode  &= ~DRAW_TRI_SMOOTH;
    drawlist_in.draw_mode  &= ~DRAW_TRI_POINTS;
    drawlist_out.draw_mode |=  DRAW_TRI_FLAT;
    drawlist_out.draw_mode &= ~DRAW_TRI_SMOOTH;
    drawlist_out.draw_mode &= ~DRAW_TRI_POINTS;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_mesh_smooth()
{
    drawlist_in.draw_mode  |=  DRAW_TRI_SMOOTH;
    drawlist_in.draw_mode  &= ~DRAW_TRI_FLAT;
    drawlist_in.draw_mode  &= ~DRAW_TRI_POINTS;
    drawlist_out.draw_mode |=  DRAW_TRI_SMOOTH;
    drawlist_out.draw_mode &= ~DRAW_TRI_FLAT;
    drawlist_out.draw_mode &= ~DRAW_TRI_POINTS;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_mesh_points()
{
    drawlist_in.draw_mode  |=  DRAW_TRI_POINTS;
    drawlist_in.draw_mode  &= ~DRAW_TRI_FLAT;
    drawlist_in.draw_mode  &= ~DRAW_TRI_SMOOTH;
    drawlist_out.draw_mode |=  DRAW_TRI_POINTS;
    drawlist_out.draw_mode &= ~DRAW_TRI_FLAT;
    drawlist_out.draw_mode &= ~DRAW_TRI_SMOOTH;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_face_color()
{
    drawlist_out.draw_mode |=  DRAW_TRI_FACECOLOR;
    drawlist_out.draw_mode &= ~DRAW_TRI_VERTCOLOR;
    drawlist_out.draw_mode &= ~DRAW_TRI_QUALITY;
    drawlist_out.draw_mode &= ~DRAW_TRI_TEXTURE1D;
    glDisable(GL_TEXTURE_1D);
    updateGL_out();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_face_quality()
{
    drawlist_out.draw_mode |=  DRAW_TRI_QUALITY;
    drawlist_out.draw_mode &= ~DRAW_TRI_FACECOLOR;
    drawlist_out.draw_mode &= ~DRAW_TRI_VERTCOLOR;
    drawlist_out.draw_mode &= ~DRAW_TRI_TEXTURE1D;
    glDisable(GL_TEXTURE_1D);
    updateGL_out();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_face_texture1D(const GLint texture)
{
    drawlist_out.draw_mode |=  DRAW_TRI_TEXTURE1D;
    drawlist_out.draw_mode &= ~DRAW_TRI_VERTCOLOR;
    drawlist_out.draw_mode &= ~DRAW_TRI_FACECOLOR;
    drawlist_out.draw_mode &= ~DRAW_TRI_QUALITY;

    if (drawlist_out.tri_text1D_id > 0) glDeleteTextures(1, &drawlist_out.tri_text1D_id);
    glGenTextures(1, &drawlist_out.tri_text1D_id);
    glBindTexture(GL_TEXTURE_1D, drawlist_out.tri_text1D_id);
    switch (texture)
    {
        case TEXTURE_ISOLINES               : glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, isolines_texture1D); break;
        case TEXTURE_QUALITY_RAMP           : glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, quality_ramp_texture1D); break;
        case TEXTURE_QUALITY_RAMP_W_ISOLINES: glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, quality_ramp_texture1D_with_isolines); break;
        default : assert("Unknown 1D Texture" && false);
    }
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R,     GL_REPEAT);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_1D);

    updateGL_out();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_face_wireframe(const bool b)
{
    if (b) drawlist_out.draw_mode |=  DRAW_SEGS;
    else   drawlist_out.draw_mode &= ~DRAW_SEGS;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_face_wireframe_color(const Color & c)
{
    edge_set_color(c); // NOTE: this will change alpha for ANY adge (both interior and boundary)
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_face_wireframe_width(const float width)
{
    drawlist_out.seg_width = width;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_face_wireframe_transparency(const float alpha)
{
    edge_set_alpha(alpha); // NOTE: this will change alpha for ANY adge (both interior and boundary)
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_cell_color()
{
    drawlist_in.draw_mode |=  DRAW_TRI_FACECOLOR;
    drawlist_in.draw_mode &= ~DRAW_TRI_VERTCOLOR;
    drawlist_in.draw_mode &= ~DRAW_TRI_QUALITY;
    drawlist_in.draw_mode &= ~DRAW_TRI_TEXTURE1D;
    glDisable(GL_TEXTURE_1D);
    updateGL_in();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_cell_quality()
{
    drawlist_in.draw_mode |=  DRAW_TRI_QUALITY;
    drawlist_in.draw_mode &= ~DRAW_TRI_FACECOLOR;
    drawlist_in.draw_mode &= ~DRAW_TRI_VERTCOLOR;
    drawlist_in.draw_mode &= ~DRAW_TRI_TEXTURE1D;
    glDisable(GL_TEXTURE_1D);
    updateGL_in();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_cell_texture1D(const GLint texture)
{
    drawlist_in.draw_mode |=  DRAW_TRI_TEXTURE1D;
    drawlist_in.draw_mode &= ~DRAW_TRI_VERTCOLOR;
    drawlist_in.draw_mode &= ~DRAW_TRI_FACECOLOR;
    drawlist_in.draw_mode &= ~DRAW_TRI_QUALITY;

    if (drawlist_in.tri_text1D_id > 0) glDeleteTextures(1, &drawlist_in.tri_text1D_id);
    glGenTextures(1, &drawlist_in.tri_text1D_id);
    glBindTexture(GL_TEXTURE_1D, drawlist_in.tri_text1D_id);
    switch (texture)
    {
        case TEXTURE_ISOLINES               : glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, isolines_texture1D); break;
        case TEXTURE_QUALITY_RAMP           : glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, quality_ramp_texture1D); break;
        case TEXTURE_QUALITY_RAMP_W_ISOLINES: glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, quality_ramp_texture1D_with_isolines); break;
        default : assert("Unknown 1D Texture" && false);
    }
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R,     GL_REPEAT);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_1D);

    updateGL_in();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_cell_wireframe(const bool b)
{
    if (b) drawlist_in.draw_mode |=  DRAW_SEGS;
    else   drawlist_in.draw_mode &= ~DRAW_SEGS;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_cell_wireframe_color(const Color & c)
{
    edge_set_color(c); // NOTE: this will change color for ANY adge (both interior and boundary)
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_cell_wireframe_width(const float width)
{
    drawlist_in.seg_width = width;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void DrawablePolyhedralmesh<M,V,E,F,C>::show_cell_wireframe_transparency(const float alpha)
{
    edge_set_alpha(alpha); // NOTE: this will change alpha for ANY adge (both interior and boundary)
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

}
