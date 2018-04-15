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
    render(drawlist);
    render(drawlist_marked);
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
        if (!this->edge_data(eid).marked) continue;

        bool invisible = true;
        for(uint fid : this->adj_e2p(eid))
        {
            if (this->poly_data(fid).visible) invisible = false;
        }
        if (invisible) continue;

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
            if (!(this->poly_data(pid).visible)) continue;

            for(uint i=0; i<this->poly_tessellation(pid).size()/3; ++i)
            {
                uint vid0 = this->poly_tessellation(pid).at(3*i+0);
                uint vid1 = this->poly_tessellation(pid).at(3*i+1);
                uint vid2 = this->poly_tessellation(pid).at(3*i+2);

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
                    drawlist.tri_v_norms.push_back(this->vert_data(vid0).normal.x());
                    drawlist.tri_v_norms.push_back(this->vert_data(vid0).normal.y());
                    drawlist.tri_v_norms.push_back(this->vert_data(vid0).normal.z());
                    drawlist.tri_v_norms.push_back(this->vert_data(vid1).normal.x());
                    drawlist.tri_v_norms.push_back(this->vert_data(vid1).normal.y());
                    drawlist.tri_v_norms.push_back(this->vert_data(vid1).normal.z());
                    drawlist.tri_v_norms.push_back(this->vert_data(vid2).normal.x());
                    drawlist.tri_v_norms.push_back(this->vert_data(vid2).normal.y());
                    drawlist.tri_v_norms.push_back(this->vert_data(vid2).normal.z());
                }
                else if (drawlist.draw_mode & DRAW_TRI_FLAT)
                {
                    drawlist.tri_v_norms.push_back(this->poly_data(pid).normal.x());
                    drawlist.tri_v_norms.push_back(this->poly_data(pid).normal.y());
                    drawlist.tri_v_norms.push_back(this->poly_data(pid).normal.z());
                    drawlist.tri_v_norms.push_back(this->poly_data(pid).normal.x());
                    drawlist.tri_v_norms.push_back(this->poly_data(pid).normal.y());
                    drawlist.tri_v_norms.push_back(this->poly_data(pid).normal.z());
                    drawlist.tri_v_norms.push_back(this->poly_data(pid).normal.x());
                    drawlist.tri_v_norms.push_back(this->poly_data(pid).normal.y());
                    drawlist.tri_v_norms.push_back(this->poly_data(pid).normal.z());
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
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.r);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.g);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.b);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.a);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.r);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.g);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.b);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.a);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.r);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.g);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.b);
                    drawlist.tri_v_colors.push_back(this->poly_data(pid).color.a);
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
                else if (drawlist.draw_mode & DRAW_TRI_QUALITY)
                {
                    float q = this->poly_data(pid).quality;
                    Color c = Color::quality2rgb(q);
                    drawlist.tri_v_colors.push_back(c.r);
                    drawlist.tri_v_colors.push_back(c.g);
                    drawlist.tri_v_colors.push_back(c.b);
                    drawlist.tri_v_colors.push_back(c.a);
                    drawlist.tri_v_colors.push_back(c.r);
                    drawlist.tri_v_colors.push_back(c.g);
                    drawlist.tri_v_colors.push_back(c.b);
                    drawlist.tri_v_colors.push_back(c.a);
                    drawlist.tri_v_colors.push_back(c.r);
                    drawlist.tri_v_colors.push_back(c.g);
                    drawlist.tri_v_colors.push_back(c.b);
                    drawlist.tri_v_colors.push_back(c.a);
                }
            }
        }

        for(uint eid=0; eid<this->num_edges(); ++eid)
        {
            bool invisible = true;
            for(uint fid : this->adj_e2p(eid))
            {
                if (this->poly_data(fid).visible) invisible = false;
            }
            if (invisible) continue;

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
        case TEXTURE_1D_ISOLINES :            texture_isolines1D(drawlist.texture);             break;
        case TEXTURE_1D_HSV_RAMP :            texture_HSV_ramp(drawlist.texture);               break;
        case TEXTURE_1D_HSV_RAMP_W_ISOLINES : texture_HSV_ramp_with_isolines(drawlist.texture); break;
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
