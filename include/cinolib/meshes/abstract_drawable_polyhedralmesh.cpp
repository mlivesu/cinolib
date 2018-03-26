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
#include <cinolib/meshes/abstract_drawable_polyhedralmesh.h>
#include <cinolib/cino_inline.h>
#include <cinolib/gl/draw_lines_tris.h>
#include <cinolib/textures/textures.h>
#include <cinolib/color.h>
#include <set>

namespace cinolib
{

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::init_drawable_stuff()
{
    slicer = MeshSlicer<Mesh>(*this);

    drawlist_in.draw_mode     = DRAW_TRIS | DRAW_TRI_FLAT | DRAW_TRI_FACECOLOR | DRAW_SEGS;
    drawlist_out.draw_mode    = DRAW_TRIS | DRAW_TRI_FLAT | DRAW_TRI_FACECOLOR | DRAW_SEGS;
    drawlist_marked.draw_mode = DRAW_TRIS | DRAW_SEGS;
    drawlist_marked.seg_width = 3;
    marked_edge_color         = Color::RED();
    marked_face_color         = Color::BLUE();

    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::draw(const float) const
{
    render(drawlist_in );
    render(drawlist_out);
    render(drawlist_marked);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::updateGL()
{
    updateGL_out();
    updateGL_in();
    updateGL_marked();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::updateGL_marked()
{
    drawlist_marked.tris.clear();
    drawlist_marked.tri_coords.clear();
    drawlist_marked.tri_v_norms.clear();
    drawlist_marked.tri_v_colors.clear();
    drawlist_marked.segs.clear();
    drawlist_marked.seg_coords.clear();
    drawlist_marked.seg_colors.clear();

    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        if (!this->face_data(fid).marked) continue;

        //assert(this->adj_f2p(fid).size()==1);
        //uint pid = this->adj_f2p(fid).front();
        //if (!(this->poly_data(pid).visible)) continue;

        for(uint i=0; i<this->face_tessellation(fid).size()/3; ++i)
        {
            uint vid0 = this->face_tessellation(fid).at(3*i+0);
            uint vid1 = this->face_tessellation(fid).at(3*i+1);
            uint vid2 = this->face_tessellation(fid).at(3*i+2);

            int base_addr = drawlist_marked.tri_coords.size()/3;

            drawlist_marked.tris.push_back(base_addr    );
            drawlist_marked.tris.push_back(base_addr + 1);
            drawlist_marked.tris.push_back(base_addr + 2);

            drawlist_marked.tri_coords.push_back(this->vert(vid0).x());
            drawlist_marked.tri_coords.push_back(this->vert(vid0).y());
            drawlist_marked.tri_coords.push_back(this->vert(vid0).z());
            drawlist_marked.tri_coords.push_back(this->vert(vid1).x());
            drawlist_marked.tri_coords.push_back(this->vert(vid1).y());
            drawlist_marked.tri_coords.push_back(this->vert(vid1).z());
            drawlist_marked.tri_coords.push_back(this->vert(vid2).x());
            drawlist_marked.tri_coords.push_back(this->vert(vid2).y());
            drawlist_marked.tri_coords.push_back(this->vert(vid2).z());

            drawlist_marked.tri_v_norms.push_back(this->face_data(fid).normal.x());
            drawlist_marked.tri_v_norms.push_back(this->face_data(fid).normal.y());
            drawlist_marked.tri_v_norms.push_back(this->face_data(fid).normal.z());
            drawlist_marked.tri_v_norms.push_back(this->face_data(fid).normal.x());
            drawlist_marked.tri_v_norms.push_back(this->face_data(fid).normal.y());
            drawlist_marked.tri_v_norms.push_back(this->face_data(fid).normal.z());
            drawlist_marked.tri_v_norms.push_back(this->face_data(fid).normal.x());
            drawlist_marked.tri_v_norms.push_back(this->face_data(fid).normal.y());
            drawlist_marked.tri_v_norms.push_back(this->face_data(fid).normal.z());

            drawlist_marked.tri_v_colors.push_back(marked_face_color.r);
            drawlist_marked.tri_v_colors.push_back(marked_face_color.g);
            drawlist_marked.tri_v_colors.push_back(marked_face_color.b);
            drawlist_marked.tri_v_colors.push_back(marked_face_color.a);
            drawlist_marked.tri_v_colors.push_back(marked_face_color.r);
            drawlist_marked.tri_v_colors.push_back(marked_face_color.g);
            drawlist_marked.tri_v_colors.push_back(marked_face_color.b);
            drawlist_marked.tri_v_colors.push_back(marked_face_color.a);
            drawlist_marked.tri_v_colors.push_back(marked_face_color.r);
            drawlist_marked.tri_v_colors.push_back(marked_face_color.g);
            drawlist_marked.tri_v_colors.push_back(marked_face_color.b);
            drawlist_marked.tri_v_colors.push_back(marked_face_color.a);
        }
    }

    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if (!this->edge_data(eid).marked) continue;

        //bool invisible = true;
        //for(uint pid : this->adj_e2p(eid))
        //{
        //    if (this->poly_data(pid).visible) invisible = false;
        //}
        //if (invisible) continue;

        vec3d vid0 = this->edge_vert(eid,0);
        vec3d vid1 = this->edge_vert(eid,1);

        int base_addr = drawlist_marked.seg_coords.size()/3;
        drawlist_marked.segs.push_back(base_addr    );
        drawlist_marked.segs.push_back(base_addr + 1);

        drawlist_marked.seg_coords.push_back(vid0.x());
        drawlist_marked.seg_coords.push_back(vid0.y());
        drawlist_marked.seg_coords.push_back(vid0.z());
        drawlist_marked.seg_coords.push_back(vid1.x());
        drawlist_marked.seg_coords.push_back(vid1.y());
        drawlist_marked.seg_coords.push_back(vid1.z());

        drawlist_marked.seg_colors.push_back(marked_edge_color.r);
        drawlist_marked.seg_colors.push_back(marked_edge_color.g);
        drawlist_marked.seg_colors.push_back(marked_edge_color.b);
        drawlist_marked.seg_colors.push_back(marked_edge_color.a);
        drawlist_marked.seg_colors.push_back(marked_edge_color.r);
        drawlist_marked.seg_colors.push_back(marked_edge_color.g);
        drawlist_marked.seg_colors.push_back(marked_edge_color.b);
        drawlist_marked.seg_colors.push_back(marked_edge_color.a);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::updateGL_out()
{
    drawlist_out.tris.clear();
    drawlist_out.tri_coords.clear();
    drawlist_out.tri_v_norms.clear();
    drawlist_out.tri_v_colors.clear();
    drawlist_out.tri_text.clear();
    drawlist_out.segs.clear();
    drawlist_out.seg_coords.clear();
    drawlist_out.seg_colors.clear();

    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        if (!this->face_is_on_srf(fid)) continue;

        assert(this->adj_f2p(fid).size()==1);
        uint pid = this->adj_f2p(fid).front();

        if (!(this->poly_data(pid).visible)) continue;

        for(uint i=0; i<this->face_tessellation(fid).size()/3; ++i)
        {
            uint vid0 = this->face_tessellation(fid).at(3*i+0);
            uint vid1 = this->face_tessellation(fid).at(3*i+1);
            uint vid2 = this->face_tessellation(fid).at(3*i+2);

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

            if (drawlist_out.draw_mode & DRAW_TRI_TEXTURE1D)
            {
                drawlist_out.tri_text.push_back(this->vert_data(vid0).uvw[0]);
                drawlist_out.tri_text.push_back(this->vert_data(vid1).uvw[0]);
                drawlist_out.tri_text.push_back(this->vert_data(vid2).uvw[0]);
            }
            else if (drawlist_out.draw_mode & DRAW_TRI_TEXTURE2D)
            {
                drawlist_out.tri_text.push_back(this->vert_data(vid0).uvw[0]*drawlist_out.texture.scaling_factor);
                drawlist_out.tri_text.push_back(this->vert_data(vid0).uvw[1]*drawlist_out.texture.scaling_factor);
                drawlist_out.tri_text.push_back(this->vert_data(vid1).uvw[0]*drawlist_out.texture.scaling_factor);
                drawlist_out.tri_text.push_back(this->vert_data(vid1).uvw[1]*drawlist_out.texture.scaling_factor);
                drawlist_out.tri_text.push_back(this->vert_data(vid2).uvw[0]*drawlist_out.texture.scaling_factor);
                drawlist_out.tri_text.push_back(this->vert_data(vid2).uvw[1]*drawlist_out.texture.scaling_factor);
            }

            if (drawlist_out.draw_mode & DRAW_TRI_FACECOLOR) // replicate f color on each vertex
            {
                drawlist_out.tri_v_colors.push_back(this->poly_data(pid).color.r);
                drawlist_out.tri_v_colors.push_back(this->poly_data(pid).color.g);
                drawlist_out.tri_v_colors.push_back(this->poly_data(pid).color.b);
                drawlist_out.tri_v_colors.push_back(this->poly_data(pid).color.a);
                drawlist_out.tri_v_colors.push_back(this->poly_data(pid).color.r);
                drawlist_out.tri_v_colors.push_back(this->poly_data(pid).color.g);
                drawlist_out.tri_v_colors.push_back(this->poly_data(pid).color.b);
                drawlist_out.tri_v_colors.push_back(this->poly_data(pid).color.a);
                drawlist_out.tri_v_colors.push_back(this->poly_data(pid).color.r);
                drawlist_out.tri_v_colors.push_back(this->poly_data(pid).color.g);
                drawlist_out.tri_v_colors.push_back(this->poly_data(pid).color.b);
                drawlist_out.tri_v_colors.push_back(this->poly_data(pid).color.a);
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
            else if (drawlist_out.draw_mode & DRAW_TRI_QUALITY)
            {
                float q = this->poly_data(pid).quality;
                Color c = Color::quality2rgb(q);
                drawlist_out.tri_v_colors.push_back(c.r);
                drawlist_out.tri_v_colors.push_back(c.g);
                drawlist_out.tri_v_colors.push_back(c.b);
                drawlist_out.tri_v_colors.push_back(c.a);
                drawlist_out.tri_v_colors.push_back(c.r);
                drawlist_out.tri_v_colors.push_back(c.g);
                drawlist_out.tri_v_colors.push_back(c.b);
                drawlist_out.tri_v_colors.push_back(c.a);
                drawlist_out.tri_v_colors.push_back(c.r);
                drawlist_out.tri_v_colors.push_back(c.g);
                drawlist_out.tri_v_colors.push_back(c.b);
                drawlist_out.tri_v_colors.push_back(c.a);
            }
        }
    }

    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        vec3d vid0 = this->edge_vert(eid,0);
        vec3d vid1 = this->edge_vert(eid,1);

        if (this->edge_is_on_srf(eid))
        {
            bool invisible = true;
            for(uint pid : this->adj_e2p(eid))
            {
                if (this->poly_data(pid).visible) invisible = false;
            }
            if (invisible) continue;

            int base_addr = drawlist_out.seg_coords.size()/3;
            drawlist_out.segs.push_back(base_addr    );
            drawlist_out.segs.push_back(base_addr + 1);

            drawlist_out.seg_coords.push_back(vid0.x());
            drawlist_out.seg_coords.push_back(vid0.y());
            drawlist_out.seg_coords.push_back(vid0.z());
            drawlist_out.seg_coords.push_back(vid1.x());
            drawlist_out.seg_coords.push_back(vid1.y());
            drawlist_out.seg_coords.push_back(vid1.z());

            drawlist_out.seg_colors.push_back(this->edge_data(eid).color.r);
            drawlist_out.seg_colors.push_back(this->edge_data(eid).color.g);
            drawlist_out.seg_colors.push_back(this->edge_data(eid).color.b);
            drawlist_out.seg_colors.push_back(this->edge_data(eid).color.a);
            drawlist_out.seg_colors.push_back(this->edge_data(eid).color.r);
            drawlist_out.seg_colors.push_back(this->edge_data(eid).color.g);
            drawlist_out.seg_colors.push_back(this->edge_data(eid).color.b);
            drawlist_out.seg_colors.push_back(this->edge_data(eid).color.a);
        }

        if (this->edge_data(eid).marked)
        {
            int base_addr = drawlist_marked.seg_coords.size()/3;
            drawlist_marked.segs.push_back(base_addr    );
            drawlist_marked.segs.push_back(base_addr + 1);

            drawlist_marked.seg_coords.push_back(vid0.x());
            drawlist_marked.seg_coords.push_back(vid0.y());
            drawlist_marked.seg_coords.push_back(vid0.z());
            drawlist_marked.seg_coords.push_back(vid1.x());
            drawlist_marked.seg_coords.push_back(vid1.y());
            drawlist_marked.seg_coords.push_back(vid1.z());

            drawlist_marked.seg_colors.push_back(marked_edge_color.r);
            drawlist_marked.seg_colors.push_back(marked_edge_color.g);
            drawlist_marked.seg_colors.push_back(marked_edge_color.b);
            drawlist_marked.seg_colors.push_back(marked_edge_color.a);
            drawlist_marked.seg_colors.push_back(marked_edge_color.r);
            drawlist_marked.seg_colors.push_back(marked_edge_color.g);
            drawlist_marked.seg_colors.push_back(marked_edge_color.b);
            drawlist_marked.seg_colors.push_back(marked_edge_color.a);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::updateGL_in()
{
    drawlist_in.tris.clear();
    drawlist_in.tri_coords.clear();
    drawlist_in.tri_v_norms.clear();
    drawlist_in.tri_v_colors.clear();
    drawlist_in.tri_text.clear();
    drawlist_in.segs.clear();
    drawlist_in.seg_coords.clear();
    drawlist_in.seg_colors.clear();

    std::set<uint> edges_to_render;

    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        if (this->face_is_on_srf(fid)) continue;

        assert(this->adj_f2p(fid).size()==2);
        std::vector<uint> visible_polys;
        for(uint pid : this->adj_f2p(fid))
        {
            if (this->poly_data(pid).visible) visible_polys.push_back(pid);
        }
        if (visible_polys.size()!=1) continue;

        uint pid   = visible_polys.front();
        bool is_CW = this->poly_face_is_CW(pid,fid);

        for(uint eid : this->adj_f2e(fid))
        {
            if (this->edge_is_on_srf(eid)) continue; // updateGL_out() will consider it
            edges_to_render.insert(eid);
        }

        for(uint i=0; i<this->face_tessellation(fid).size()/3; ++i)
        {
            uint vid0 = this->face_tessellation(fid).at(3*i+0);
            uint vid1 = this->face_tessellation(fid).at(3*i+1);
            uint vid2 = this->face_tessellation(fid).at(3*i+2);
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

            if (drawlist_in.draw_mode & DRAW_TRI_TEXTURE1D)
            {
                drawlist_in.tri_text.push_back(this->vert_data(vid0).uvw[0]);
                drawlist_in.tri_text.push_back(this->vert_data(vid1).uvw[0]);
                drawlist_in.tri_text.push_back(this->vert_data(vid2).uvw[0]);
            }
            else if (drawlist_in.draw_mode & DRAW_TRI_TEXTURE2D)
            {
                drawlist_in.tri_text.push_back(this->vert_data(vid0).uvw[0]*drawlist_in.texture.scaling_factor);
                drawlist_in.tri_text.push_back(this->vert_data(vid0).uvw[1]*drawlist_in.texture.scaling_factor);
                drawlist_in.tri_text.push_back(this->vert_data(vid1).uvw[0]*drawlist_in.texture.scaling_factor);
                drawlist_in.tri_text.push_back(this->vert_data(vid1).uvw[1]*drawlist_in.texture.scaling_factor);
                drawlist_in.tri_text.push_back(this->vert_data(vid2).uvw[0]*drawlist_in.texture.scaling_factor);
                drawlist_in.tri_text.push_back(this->vert_data(vid2).uvw[1]*drawlist_in.texture.scaling_factor);
            }

            if (drawlist_in.draw_mode & DRAW_TRI_FACECOLOR) // replicate f color on each vertex
            {
                drawlist_in.tri_v_colors.push_back(this->poly_data(pid).color.r);
                drawlist_in.tri_v_colors.push_back(this->poly_data(pid).color.g);
                drawlist_in.tri_v_colors.push_back(this->poly_data(pid).color.b);
                drawlist_in.tri_v_colors.push_back(this->poly_data(pid).color.a);
                drawlist_in.tri_v_colors.push_back(this->poly_data(pid).color.r);
                drawlist_in.tri_v_colors.push_back(this->poly_data(pid).color.g);
                drawlist_in.tri_v_colors.push_back(this->poly_data(pid).color.b);
                drawlist_in.tri_v_colors.push_back(this->poly_data(pid).color.a);
                drawlist_in.tri_v_colors.push_back(this->poly_data(pid).color.r);
                drawlist_in.tri_v_colors.push_back(this->poly_data(pid).color.g);
                drawlist_in.tri_v_colors.push_back(this->poly_data(pid).color.b);
                drawlist_in.tri_v_colors.push_back(this->poly_data(pid).color.a);
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
            else if (drawlist_in.draw_mode & DRAW_TRI_QUALITY)
            {
                float q = this->poly_data(pid).quality;
                Color c = Color::quality2rgb(q);
                drawlist_in.tri_v_colors.push_back(c.r);
                drawlist_in.tri_v_colors.push_back(c.g);
                drawlist_in.tri_v_colors.push_back(c.b);
                drawlist_in.tri_v_colors.push_back(c.a);
                drawlist_in.tri_v_colors.push_back(c.r);
                drawlist_in.tri_v_colors.push_back(c.g);
                drawlist_in.tri_v_colors.push_back(c.b);
                drawlist_in.tri_v_colors.push_back(c.a);
                drawlist_in.tri_v_colors.push_back(c.r);
                drawlist_in.tri_v_colors.push_back(c.g);
                drawlist_in.tri_v_colors.push_back(c.b);
                drawlist_in.tri_v_colors.push_back(c.a);
            }
        }
    }

    for(uint eid : edges_to_render)
    {
        uint base_addr = drawlist_in.seg_coords.size()/3;
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

        drawlist_in.seg_colors.push_back(this->edge_data(eid).color.r);
        drawlist_in.seg_colors.push_back(this->edge_data(eid).color.g);
        drawlist_in.seg_colors.push_back(this->edge_data(eid).color.b);
        drawlist_in.seg_colors.push_back(this->edge_data(eid).color.a);
        drawlist_in.seg_colors.push_back(this->edge_data(eid).color.r);
        drawlist_in.seg_colors.push_back(this->edge_data(eid).color.g);
        drawlist_in.seg_colors.push_back(this->edge_data(eid).color.b);
        drawlist_in.seg_colors.push_back(this->edge_data(eid).color.a);

//        if (this->edge_data(eid).marked && drawlist_in.draw_mode & DRAW_MARKED_SEGS)
//        {
//            int base_addr = drawlist_in.marked_seg_coords.size()/3;
//            drawlist_in.marked_segs.push_back(base_addr    );
//            drawlist_in.marked_segs.push_back(base_addr + 1);

//            drawlist_in.marked_seg_coords.push_back(vid0.x());
//            drawlist_in.marked_seg_coords.push_back(vid0.y());
//            drawlist_in.marked_seg_coords.push_back(vid0.z());
//            drawlist_in.marked_seg_coords.push_back(vid1.x());
//            drawlist_in.marked_seg_coords.push_back(vid1.y());
//            drawlist_in.marked_seg_coords.push_back(vid1.z());
//        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::slice(const SlicerState & s)
{
    slicer.update(*this, s); // update per element visibility flags
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::slicer_reset()   // either AND or OR
{
    slicer.reset(*this);
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_mesh(const bool b)
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

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_mesh_flat()
{
    drawlist_in.draw_mode  |=  DRAW_TRI_FLAT;
    drawlist_in.draw_mode  &= ~DRAW_TRI_SMOOTH;
    drawlist_in.draw_mode  &= ~DRAW_TRI_POINTS;

    drawlist_out.draw_mode |=  DRAW_TRI_FLAT;
    drawlist_out.draw_mode &= ~DRAW_TRI_SMOOTH;
    drawlist_out.draw_mode &= ~DRAW_TRI_POINTS;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_mesh_smooth()
{
    drawlist_in.draw_mode  |=  DRAW_TRI_SMOOTH;
    drawlist_in.draw_mode  &= ~DRAW_TRI_FLAT;
    drawlist_in.draw_mode  &= ~DRAW_TRI_POINTS;

    drawlist_out.draw_mode |=  DRAW_TRI_SMOOTH;
    drawlist_out.draw_mode &= ~DRAW_TRI_FLAT;
    drawlist_out.draw_mode &= ~DRAW_TRI_POINTS;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_mesh_points()
{
    drawlist_in.draw_mode  |=  DRAW_TRI_POINTS;
    drawlist_in.draw_mode  &= ~DRAW_TRI_FLAT;
    drawlist_in.draw_mode  &= ~DRAW_TRI_SMOOTH;

    drawlist_out.draw_mode |=  DRAW_TRI_POINTS;
    drawlist_out.draw_mode &= ~DRAW_TRI_FLAT;
    drawlist_out.draw_mode &= ~DRAW_TRI_SMOOTH;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_out_vert_color()
{
    drawlist_out.draw_mode |=  DRAW_TRI_VERTCOLOR;
    drawlist_out.draw_mode &= ~DRAW_TRI_FACECOLOR;
    drawlist_out.draw_mode &= ~DRAW_TRI_QUALITY;
    drawlist_out.draw_mode &= ~DRAW_TRI_TEXTURE1D;
    drawlist_out.draw_mode &= ~DRAW_TRI_TEXTURE2D;
    updateGL_out();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_in_vert_color()
{
    drawlist_in.draw_mode |=  DRAW_TRI_VERTCOLOR;
    drawlist_in.draw_mode &= ~DRAW_TRI_FACECOLOR;
    drawlist_in.draw_mode &= ~DRAW_TRI_QUALITY;
    drawlist_in.draw_mode &= ~DRAW_TRI_TEXTURE1D;
    drawlist_in.draw_mode &= ~DRAW_TRI_TEXTURE2D;
    updateGL_in();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_out_face_color()
{
    assert(false && "Per face color visualization not implemented yet!");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_out_poly_color()
{
    drawlist_out.draw_mode |=  DRAW_TRI_FACECOLOR;
    drawlist_out.draw_mode &= ~DRAW_TRI_VERTCOLOR;
    drawlist_out.draw_mode &= ~DRAW_TRI_QUALITY;
    drawlist_out.draw_mode &= ~DRAW_TRI_TEXTURE1D;
    drawlist_out.draw_mode &= ~DRAW_TRI_TEXTURE2D;
    updateGL_out();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_out_poly_quality()
{
    drawlist_out.draw_mode |=  DRAW_TRI_QUALITY;
    drawlist_out.draw_mode &= ~DRAW_TRI_FACECOLOR;
    drawlist_out.draw_mode &= ~DRAW_TRI_VERTCOLOR;
    drawlist_out.draw_mode &= ~DRAW_TRI_TEXTURE1D;
    drawlist_out.draw_mode &= ~DRAW_TRI_TEXTURE2D;
    updateGL_out();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_out_texture1D(const int tex_type)
{
    drawlist_out.draw_mode |=  DRAW_TRI_TEXTURE1D;
    drawlist_out.draw_mode &= ~DRAW_TRI_TEXTURE2D;
    drawlist_out.draw_mode &= ~DRAW_TRI_VERTCOLOR;
    drawlist_out.draw_mode &= ~DRAW_TRI_FACECOLOR;
    drawlist_out.draw_mode &= ~DRAW_TRI_QUALITY;

    drawlist_out.texture.type = tex_type;
    switch (tex_type)
    {
        case TEXTURE_1D_ISOLINES :            texture_isolines1D(drawlist_out.texture);             break;
        case TEXTURE_1D_HSV_RAMP :            texture_HSV_ramp(drawlist_out.texture);               break;
        case TEXTURE_1D_HSV_RAMP_W_ISOLINES : texture_HSV_ramp_with_isolines(drawlist_out.texture); break;
        default: assert("Unknown Texture!" && false);
    }
    updateGL_out();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_out_texture2D(const int tex_type, const double tex_unit_scalar, const char *bitmap)
{
    drawlist_out.draw_mode |=  DRAW_TRI_TEXTURE2D;
    drawlist_out.draw_mode &= ~DRAW_TRI_TEXTURE1D;
    drawlist_out.draw_mode &= ~DRAW_TRI_VERTCOLOR;
    drawlist_out.draw_mode &= ~DRAW_TRI_FACECOLOR;
    drawlist_out.draw_mode &= ~DRAW_TRI_QUALITY;

    drawlist_out.texture.type           = tex_type;
    drawlist_out.texture.scaling_factor = tex_unit_scalar;
    switch (tex_type)
    {
        case TEXTURE_2D_CHECKERBOARD : texture_checkerboard(drawlist_out.texture);   break;
        case TEXTURE_2D_ISOLINES:      texture_isolines2D(drawlist_out.texture);     break;
        case TEXTURE_2D_BITMAP:        texture_bitmap(drawlist_out.texture, bitmap); break;
        default: assert("Unknown Texture!" && false);
    }
    updateGL_out();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_out_wireframe(const bool b)
{
    if (b) drawlist_out.draw_mode |=  DRAW_SEGS;
    else   drawlist_out.draw_mode &= ~DRAW_SEGS;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_out_wireframe_color(const Color & c)
{
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if (this->edge_is_on_srf(eid)) this->edge_data(eid).color = c;
    }
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_out_wireframe_width(const float width)
{
    drawlist_out.seg_width = width;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_out_wireframe_transparency(const float alpha)
{
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if (this->edge_is_on_srf(eid)) this->edge_data(eid).color.a = alpha;
    }
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_in_face_color()
{
    assert(false && "Per face color visualization not implemented yet!");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_in_poly_color()
{
    drawlist_in.draw_mode |=  DRAW_TRI_FACECOLOR;
    drawlist_in.draw_mode &= ~DRAW_TRI_VERTCOLOR;
    drawlist_in.draw_mode &= ~DRAW_TRI_QUALITY;
    drawlist_in.draw_mode &= ~DRAW_TRI_TEXTURE1D;
    drawlist_in.draw_mode &= ~DRAW_TRI_TEXTURE2D;
    updateGL_in();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_in_poly_quality()
{
    drawlist_in.draw_mode |=  DRAW_TRI_QUALITY;
    drawlist_in.draw_mode &= ~DRAW_TRI_FACECOLOR;
    drawlist_in.draw_mode &= ~DRAW_TRI_VERTCOLOR;
    drawlist_in.draw_mode &= ~DRAW_TRI_TEXTURE1D;
    drawlist_in.draw_mode &= ~DRAW_TRI_TEXTURE2D;
    updateGL_in();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_in_texture1D(const int tex_type)
{
    drawlist_in.draw_mode |=  DRAW_TRI_TEXTURE1D;
    drawlist_in.draw_mode &= ~DRAW_TRI_TEXTURE2D;
    drawlist_in.draw_mode &= ~DRAW_TRI_VERTCOLOR;
    drawlist_in.draw_mode &= ~DRAW_TRI_FACECOLOR;
    drawlist_in.draw_mode &= ~DRAW_TRI_QUALITY;

    drawlist_in.texture.type = tex_type;
    switch (tex_type)
    {
        case TEXTURE_1D_ISOLINES :            texture_isolines1D(drawlist_in.texture);             break;
        case TEXTURE_1D_HSV_RAMP :            texture_HSV_ramp(drawlist_in.texture);               break;
        case TEXTURE_1D_HSV_RAMP_W_ISOLINES : texture_HSV_ramp_with_isolines(drawlist_in.texture); break;
        default: assert("Unknown Texture!" && false);
    }
    updateGL_in();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_in_texture2D(const int tex_type, const double tex_unit_scalar, const char *bitmap)
{
    drawlist_in.draw_mode |=  DRAW_TRI_TEXTURE2D;
    drawlist_in.draw_mode &= ~DRAW_TRI_TEXTURE1D;
    drawlist_in.draw_mode &= ~DRAW_TRI_VERTCOLOR;
    drawlist_in.draw_mode &= ~DRAW_TRI_FACECOLOR;
    drawlist_in.draw_mode &= ~DRAW_TRI_QUALITY;

    drawlist_in.texture.type           = tex_type;
    drawlist_in.texture.scaling_factor = tex_unit_scalar;
    switch (tex_type)
    {
        case TEXTURE_2D_CHECKERBOARD : texture_checkerboard(drawlist_in.texture);   break;
        case TEXTURE_2D_ISOLINES:      texture_isolines2D(drawlist_in.texture);     break;
        case TEXTURE_2D_BITMAP:        texture_bitmap(drawlist_in.texture, bitmap); break;
        default: assert("Unknown Texture!" && false);
    }
    updateGL_in();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_in_wireframe(const bool b)
{
    if (b) drawlist_in.draw_mode |=  DRAW_SEGS;
    else   drawlist_in.draw_mode &= ~DRAW_SEGS;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_in_wireframe_color(const Color & c)
{
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if (!this->edge_is_on_srf(eid)) this->edge_data(eid).color = c;
    }
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_in_wireframe_width(const float width)
{
    drawlist_in.seg_width = width;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_in_wireframe_transparency(const float alpha)
{
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if (!this->edge_is_on_srf(eid)) this->edge_data(eid).color.a = alpha;
    }
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_marked_edge(const bool b)
{
    if (b) drawlist_marked.draw_mode |=  DRAW_SEGS;
    else   drawlist_marked.draw_mode &= ~DRAW_SEGS;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_marked_edge_color(const Color & c)
{
    marked_edge_color = c;
    updateGL_marked();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_marked_edge_width(const float width)
{
    drawlist_marked.seg_width = width;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_marked_edge_transparency(const float alpha)
{
    marked_edge_color.a = alpha;
    updateGL_marked();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_marked_face(const bool b)
{
    if (b) drawlist_marked.draw_mode |=  DRAW_TRIS;
    else   drawlist_marked.draw_mode &= ~DRAW_TRIS;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_marked_face_color(const Color & c)
{
    marked_face_color = c;
    updateGL_marked();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolyhedralMesh<Mesh>::show_marked_face_transparency(const float alpha)
{
    marked_face_color.a = alpha;
    updateGL_marked();
}

}
