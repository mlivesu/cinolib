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

    drawlist.draw_mode = DRAW_TRIS | DRAW_TRI_SMOOTH | DRAW_TRI_FACECOLOR | DRAW_MARKED_SEGS;

    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::draw(const float) const
{
    render(drawlist);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::updateGL()
{
    drawlist.tri_coords.clear();
    drawlist.tris.clear();
    drawlist.tri_v_norms.clear();
    drawlist.tri_v_colors.clear();
    drawlist.tri_text.clear();
    drawlist.segs.clear();
    drawlist.seg_coords.clear();
    drawlist.seg_colors.clear();
    drawlist.marked_segs.clear();
    drawlist.marked_seg_coords.clear();

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

            drawlist.tri_v_norms.push_back(this->vert_data(vid0).normal.x());
            drawlist.tri_v_norms.push_back(this->vert_data(vid0).normal.y());
            drawlist.tri_v_norms.push_back(this->vert_data(vid0).normal.z());
            drawlist.tri_v_norms.push_back(this->vert_data(vid1).normal.x());
            drawlist.tri_v_norms.push_back(this->vert_data(vid1).normal.y());
            drawlist.tri_v_norms.push_back(this->vert_data(vid1).normal.z());
            drawlist.tri_v_norms.push_back(this->vert_data(vid2).normal.x());
            drawlist.tri_v_norms.push_back(this->vert_data(vid2).normal.y());
            drawlist.tri_v_norms.push_back(this->vert_data(vid2).normal.z());

            if (drawlist.draw_mode & DRAW_TRI_TEXTURE1D)
            {
                drawlist.tri_text.push_back(this->vert_data(vid0).uvw[0]);
                drawlist.tri_text.push_back(this->vert_data(vid1).uvw[0]);
                drawlist.tri_text.push_back(this->vert_data(vid2).uvw[0]);
            }
            else if (drawlist.draw_mode & DRAW_TRI_TEXTURE2D)
            {
                drawlist.tri_text.push_back(this->vert_data(vid0).uvw[0]*drawlist.tri_text_unit_scalar);
                drawlist.tri_text.push_back(this->vert_data(vid0).uvw[1]*drawlist.tri_text_unit_scalar);
                drawlist.tri_text.push_back(this->vert_data(vid1).uvw[0]*drawlist.tri_text_unit_scalar);
                drawlist.tri_text.push_back(this->vert_data(vid1).uvw[1]*drawlist.tri_text_unit_scalar);
                drawlist.tri_text.push_back(this->vert_data(vid2).uvw[0]*drawlist.tri_text_unit_scalar);
                drawlist.tri_text.push_back(this->vert_data(vid2).uvw[1]*drawlist.tri_text_unit_scalar);
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

        if (this->edge_data(eid).marked && drawlist.draw_mode & DRAW_MARKED_SEGS)
        {
            int base_addr = drawlist.marked_seg_coords.size()/3;
            drawlist.marked_segs.push_back(base_addr    );
            drawlist.marked_segs.push_back(base_addr + 1);

            drawlist.marked_seg_coords.push_back(vid0.x());
            drawlist.marked_seg_coords.push_back(vid0.y());
            drawlist.marked_seg_coords.push_back(vid0.z());
            drawlist.marked_seg_coords.push_back(vid1.x());
            drawlist.marked_seg_coords.push_back(vid1.y());
            drawlist.marked_seg_coords.push_back(vid1.z());
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
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_mesh_smooth()
{
    drawlist.draw_mode |=  DRAW_TRI_SMOOTH;
    drawlist.draw_mode &= ~DRAW_TRI_FLAT;
    drawlist.draw_mode &= ~DRAW_TRI_POINTS;
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

    load_texture(drawlist.tri_text_id, tex_type);
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

    drawlist.tri_text_unit_scalar = tex_unit_scalar;
    load_texture(drawlist.tri_text_id, tex_type, bitmap);
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
void AbstractDrawablePolygonMesh<Mesh>::show_edge_marked(const bool b)
{
    if (b) drawlist.draw_mode |=  DRAW_MARKED_SEGS;
    else   drawlist.draw_mode &= ~DRAW_MARKED_SEGS;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_edge_marked_color(const Color & c)
{
    drawlist.marked_seg_color = c;
    updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_edge_marked_width(const float width)
{
    drawlist.marked_seg_width = width;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void AbstractDrawablePolygonMesh<Mesh>::show_edge_marked_transparency(const float alpha)
{
    drawlist.marked_seg_color.a = alpha;
    updateGL();
}

}
