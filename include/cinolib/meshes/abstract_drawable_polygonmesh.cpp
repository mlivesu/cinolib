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
#include <cinolib/meshes/abstract_drawable_polygonmesh.h>
#include <cinolib/cino_inline.h>
#include <cinolib/gl/draw_lines_tris.h>
#include <cinolib/textures/textures.h>
#include <cinolib/color.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::init_drawable_stuff()
{
    slicer = MeshSlicer<Mesh>(*this);

    drawlist.draw_mode        = DRAW_TRIS | DRAW_TRI_SMOOTH | DRAW_TRI_FACECOLOR | DRAW_SEGS;
    drawlist_marked.draw_mode = DRAW_SEGS;
    drawlist_marked.seg_width = 3;
    marked_edge_color         = Color::RED();

    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::draw(const float) const
{
    render(drawlist_marked);
    render(drawlist);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::updateGL()
{
    updateGL_mesh();
    updateGL_marked();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::updateGL_marked()
{
    drawlist_marked.segs.clear();
    drawlist_marked.seg_coords.clear();
    drawlist_marked.seg_colors.clear();

    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if(!this->edge_data(eid).flags[MARKED]) continue;

        bool hidden = true;
        for(uint pid : this->adj_e2p(eid))
        {
            if(!this->poly_data(pid).flags[HIDDEN])
            {
                hidden = false;
                break;
            }
        }
        if(hidden) continue;

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
void AbstractDrawablePolygonMesh<Mesh>::updateGL_mesh()
{
    drawlist.tri_coords.clear();
    drawlist.tris.clear();
    drawlist.tri_v_norms.clear();
    drawlist.tri_v_colors.clear();
    drawlist.tri_text.clear();
    drawlist.segs.clear();
    drawlist.seg_coords.clear();
    drawlist.seg_colors.clear();

    if (this->num_polys() == 0) // for point clouds
    {
        drawlist.tri_coords.reserve(this->num_verts()*3);
        drawlist.tri_v_colors.reserve(this->num_verts()*4);
        for(uint vid=0; vid<this->num_verts(); ++vid)
        {
            drawlist.tri_coords.push_back(this->vert(vid).x());
            drawlist.tri_coords.push_back(this->vert(vid).y());
            drawlist.tri_coords.push_back(this->vert(vid).z());

            drawlist.tri_v_colors.push_back(this->vert_data(vid).color.r);
            drawlist.tri_v_colors.push_back(this->vert_data(vid).color.g);
            drawlist.tri_v_colors.push_back(this->vert_data(vid).color.b);
            drawlist.tri_v_colors.push_back(this->vert_data(vid).color.a);
        }
    }
    else
    {
        for(uint pid=0; pid<this->num_polys(); ++pid)
        {
            if (this->poly_data(pid).flags[HIDDEN]) continue;

            vec3d n = this->poly_data(pid).normal;

            for(uint i=0; i<this->poly_tessellation(pid).size()/3; ++i)
            {
                uint vid0 = this->poly_tessellation(pid).at(3*i+0);
                uint vid1 = this->poly_tessellation(pid).at(3*i+1);
                uint vid2 = this->poly_tessellation(pid).at(3*i+2);

                // average AO with adjacent visible faces having dihedral angle lower than 60 degrees
                auto  vid0_vis_pids = this->vert_adj_visible_polys(vid0, n, 60.0);
                auto  vid1_vis_pids = this->vert_adj_visible_polys(vid1, n, 60.0);
                auto  vid2_vis_pids = this->vert_adj_visible_polys(vid2, n, 60.0);
                float AO_vid0 = 0.0;
                float AO_vid1 = 0.0;
                float AO_vid2 = 0.0;
                for(uint pid : vid0_vis_pids) AO_vid0 += this->poly_data(pid).AO*AO_alpha + (1.0 - AO_alpha);
                for(uint pid : vid1_vis_pids) AO_vid1 += this->poly_data(pid).AO*AO_alpha + (1.0 - AO_alpha);
                for(uint pid : vid2_vis_pids) AO_vid2 += this->poly_data(pid).AO*AO_alpha + (1.0 - AO_alpha);
                AO_vid0 /= static_cast<float>(vid0_vis_pids.size());
                AO_vid1 /= static_cast<float>(vid1_vis_pids.size());
                AO_vid2 /= static_cast<float>(vid2_vis_pids.size());

                uint base_addr = drawlist.tri_coords.size()/3;

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

                if (drawlist.draw_mode & DRAW_TRI_SMOOTH)
                {
                    // average normals with adjacent visible faces having dihedral angle lower than 60 degrees
                    vec3d n_vid0(0,0,0);
                    vec3d n_vid1(0,0,0);
                    vec3d n_vid2(0,0,0);
                    for(uint pid : vid0_vis_pids) n_vid0 += this->poly_data(pid).normal;
                    for(uint pid : vid1_vis_pids) n_vid1 += this->poly_data(pid).normal;
                    for(uint pid : vid2_vis_pids) n_vid2 += this->poly_data(pid).normal;
                    n_vid0 /= static_cast<double>(vid0_vis_pids.size());
                    n_vid1 /= static_cast<double>(vid1_vis_pids.size());
                    n_vid2 /= static_cast<double>(vid2_vis_pids.size());

                    drawlist.tri_v_norms.push_back(n_vid0.x());
                    drawlist.tri_v_norms.push_back(n_vid0.y());
                    drawlist.tri_v_norms.push_back(n_vid0.z());
                    drawlist.tri_v_norms.push_back(n_vid1.x());
                    drawlist.tri_v_norms.push_back(n_vid1.y());
                    drawlist.tri_v_norms.push_back(n_vid1.z());
                    drawlist.tri_v_norms.push_back(n_vid2.x());
                    drawlist.tri_v_norms.push_back(n_vid2.y());
                    drawlist.tri_v_norms.push_back(n_vid2.z());
                }
                else if (drawlist.draw_mode & DRAW_TRI_FLAT)
                {
                    drawlist.tri_v_norms.push_back(n.x());
                    drawlist.tri_v_norms.push_back(n.y());
                    drawlist.tri_v_norms.push_back(n.z());
                    drawlist.tri_v_norms.push_back(n.x());
                    drawlist.tri_v_norms.push_back(n.y());
                    drawlist.tri_v_norms.push_back(n.z());
                    drawlist.tri_v_norms.push_back(n.x());
                    drawlist.tri_v_norms.push_back(n.y());
                    drawlist.tri_v_norms.push_back(n.z());
                }

                if (drawlist.draw_mode & DRAW_TRI_TEXTURE1D)
                {
                    drawlist.tri_text.push_back(this->vert_data(vid0).uvw[0]);
                    drawlist.tri_text.push_back(this->vert_data(vid1).uvw[0]);
                    drawlist.tri_text.push_back(this->vert_data(vid2).uvw[0]);
                }
                else if (drawlist.draw_mode & DRAW_TRI_TEXTURE2D)
                {
                    drawlist.tri_text.push_back(this->vert_data(vid0).uvw[0]*drawlist.texture.scaling_factor);
                    drawlist.tri_text.push_back(this->vert_data(vid0).uvw[1]*drawlist.texture.scaling_factor);
                    drawlist.tri_text.push_back(this->vert_data(vid1).uvw[0]*drawlist.texture.scaling_factor);
                    drawlist.tri_text.push_back(this->vert_data(vid1).uvw[1]*drawlist.texture.scaling_factor);
                    drawlist.tri_text.push_back(this->vert_data(vid2).uvw[0]*drawlist.texture.scaling_factor);
                    drawlist.tri_text.push_back(this->vert_data(vid2).uvw[1]*drawlist.texture.scaling_factor);
                }

                if (drawlist.draw_mode & DRAW_TRI_FACECOLOR) // replicate f color on each vertex
                {
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.r*AO_vid0);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.g*AO_vid0);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.b*AO_vid0);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.a);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.r*AO_vid1);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.g*AO_vid1);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.b*AO_vid1);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.a);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.r*AO_vid2);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.g*AO_vid2);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.b*AO_vid2);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.a);
                }
                else if (drawlist.draw_mode & DRAW_TRI_VERTCOLOR)
                {
                    drawlist.tri_v_colors.push_back(this->vert_data(vid0).color.r*AO_vid0);
                    drawlist.tri_v_colors.push_back(this->vert_data(vid0).color.g*AO_vid0);
                    drawlist.tri_v_colors.push_back(this->vert_data(vid0).color.b*AO_vid0);
                    drawlist.tri_v_colors.push_back(this->vert_data(vid0).color.a);
                    drawlist.tri_v_colors.push_back(this->vert_data(vid1).color.r*AO_vid1);
                    drawlist.tri_v_colors.push_back(this->vert_data(vid1).color.g*AO_vid1);
                    drawlist.tri_v_colors.push_back(this->vert_data(vid1).color.b*AO_vid1);
                    drawlist.tri_v_colors.push_back(this->vert_data(vid1).color.a);
                    drawlist.tri_v_colors.push_back(this->vert_data(vid2).color.r*AO_vid2);
                    drawlist.tri_v_colors.push_back(this->vert_data(vid2).color.g*AO_vid2);
                    drawlist.tri_v_colors.push_back(this->vert_data(vid2).color.b*AO_vid2);
                    drawlist.tri_v_colors.push_back(this->vert_data(vid2).color.a);
                }
                else if (drawlist.draw_mode & DRAW_TRI_QUALITY)
                {
                    float q = this->poly_data(pid).quality;
                    Color c = Color::red_white_blue_ramp_01(q);
                    drawlist.tri_v_colors.push_back(c.r*AO_vid0);
                    drawlist.tri_v_colors.push_back(c.g*AO_vid0);
                    drawlist.tri_v_colors.push_back(c.b*AO_vid0);
                    drawlist.tri_v_colors.push_back(c.a);
                    drawlist.tri_v_colors.push_back(c.r*AO_vid1);
                    drawlist.tri_v_colors.push_back(c.g*AO_vid1);
                    drawlist.tri_v_colors.push_back(c.b*AO_vid1);
                    drawlist.tri_v_colors.push_back(c.a);
                    drawlist.tri_v_colors.push_back(c.r*AO_vid2);
                    drawlist.tri_v_colors.push_back(c.g*AO_vid2);
                    drawlist.tri_v_colors.push_back(c.b*AO_vid2);
                    drawlist.tri_v_colors.push_back(c.a);
                }
            }
        }

        for(uint eid=0; eid<this->num_edges(); ++eid)
        {
            bool hidden = true;
            for(uint pid : this->adj_e2p(eid))
            {
                if(!this->poly_data(pid).flags[HIDDEN])
                {
                    hidden = false;
                    break;
                }
            }
            if(hidden) continue;

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

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::slice(const SlicerState & s)
{
    slicer.update(*this, s); // update per element visibility flags
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::slicer_reset()
{
    slicer.reset(*this);
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_mesh(const bool b)
{
    if (b) drawlist.draw_mode |=  DRAW_TRIS;
    else   drawlist.draw_mode &= ~DRAW_TRIS;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_AO_alpha(const float alpha)
{
    AO_alpha = alpha;
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_mesh_flat()
{
    drawlist.draw_mode |=  DRAW_TRI_FLAT;
    drawlist.draw_mode &= ~DRAW_TRI_SMOOTH;
    drawlist.draw_mode &= ~DRAW_TRI_POINTS;    
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_mesh_smooth()
{
    drawlist.draw_mode |=  DRAW_TRI_SMOOTH;
    drawlist.draw_mode &= ~DRAW_TRI_FLAT;
    drawlist.draw_mode &= ~DRAW_TRI_POINTS;
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_mesh_points()
{
    drawlist.draw_mode |=  DRAW_TRI_POINTS;
    drawlist.draw_mode &= ~DRAW_TRI_FLAT;
    drawlist.draw_mode &= ~DRAW_TRI_SMOOTH;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_vert_color()
{
    drawlist.draw_mode |=  DRAW_TRI_VERTCOLOR;
    drawlist.draw_mode &= ~DRAW_TRI_FACECOLOR;
    drawlist.draw_mode &= ~DRAW_TRI_QUALITY;
    drawlist.draw_mode &= ~DRAW_TRI_TEXTURE1D;
    drawlist.draw_mode &= ~DRAW_TRI_TEXTURE2D;
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_poly_color()
{
    drawlist.draw_mode |=  DRAW_TRI_FACECOLOR;
    drawlist.draw_mode &= ~DRAW_TRI_VERTCOLOR;
    drawlist.draw_mode &= ~DRAW_TRI_QUALITY;
    drawlist.draw_mode &= ~DRAW_TRI_TEXTURE1D;
    drawlist.draw_mode &= ~DRAW_TRI_TEXTURE2D;
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_texture1D(const int tex_type)
{
    drawlist.draw_mode |=  DRAW_TRI_TEXTURE1D;
    drawlist.draw_mode &= ~DRAW_TRI_TEXTURE2D;
    drawlist.draw_mode &= ~DRAW_TRI_VERTCOLOR;
    drawlist.draw_mode &= ~DRAW_TRI_FACECOLOR;
    drawlist.draw_mode &= ~DRAW_TRI_QUALITY;

    drawlist.texture.type = tex_type;
    switch (tex_type)
    {
        case TEXTURE_1D_ISOLINES :          texture_isolines1D(drawlist.texture);           break;
        case TEXTURE_1D_HSV :               texture_HSV(drawlist.texture);                  break;
        case TEXTURE_1D_HSV_W_ISOLINES :    texture_HSV_with_isolines(drawlist.texture);    break;
        case TEXTURE_1D_PARULA :            texture_parula(drawlist.texture);               break;
        case TEXTURE_1D_PARULA_W_ISOLINES : texture_parula_with_isolines(drawlist.texture); break;
        default: assert("Unknown Texture!" && false);
    }
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_texture2D(const int tex_type, const double tex_unit_scalar, const char *bitmap)
{
    drawlist.draw_mode |=  DRAW_TRI_TEXTURE2D;
    drawlist.draw_mode &= ~DRAW_TRI_TEXTURE1D;
    drawlist.draw_mode &= ~DRAW_TRI_VERTCOLOR;
    drawlist.draw_mode &= ~DRAW_TRI_FACECOLOR;
    drawlist.draw_mode &= ~DRAW_TRI_QUALITY;

    drawlist.texture.type           = tex_type;
    drawlist.texture.scaling_factor = tex_unit_scalar;
    switch (tex_type)
    {
        case TEXTURE_2D_CHECKERBOARD : texture_checkerboard(drawlist.texture);   break;
        case TEXTURE_2D_ISOLINES:      texture_isolines2D(drawlist.texture);     break;
        case TEXTURE_2D_BITMAP:        texture_bitmap(drawlist.texture, bitmap); break;
        default: assert("Unknown Texture!" && false);
    }
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_wireframe(const bool b)
{
    if (b) drawlist.draw_mode |=  DRAW_SEGS;
    else   drawlist.draw_mode &= ~DRAW_SEGS;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_wireframe_color(const Color & c)
{
    this->edge_set_color(c); // NOTE: this will change alpha for ANY adge (both interior and boundary)
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_wireframe_width(const float width)
{
    drawlist.seg_width = width;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_wireframe_transparency(const float alpha)
{
    this->edge_set_alpha(alpha); // NOTE: this will change alpha for ANY adge (both interior and boundary)
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_marked_edge(const bool b)
{
    if (b) drawlist_marked.draw_mode |=  DRAW_SEGS;
    else   drawlist_marked.draw_mode &= ~DRAW_SEGS;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_marked_edge_color(const Color & c)
{
    marked_edge_color = c;
    updateGL_marked();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_marked_edge_width(const float width)
{
    drawlist_marked.seg_width = width;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_marked_edge_transparency(const float alpha)
{
    marked_edge_color.a = alpha;
    updateGL_marked();
}

}
