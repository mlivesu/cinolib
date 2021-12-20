/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2021: Marco Livesu                                              *
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
#ifndef CINO_SURFACE_MESH_CONTROLS_H
#define CINO_SURFACE_MESH_CONTROLS_H

#include <cinolib/cino_inline.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/visual_control.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/load_texture.h>
#include <cinolib/drawable_isocontour.h>
#include <cinolib/drawable_vector_field.h>

namespace cinolib
{

template<class Mesh>
class SurfaceMeshControls : public VisualControl
{
    Mesh     * m   = nullptr;
    GLcanvas * gui = nullptr;

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    typedef typename Mesh::M_type M;
    typedef typename Mesh::V_type V;
    typedef typename Mesh::E_type E;
    typedef typename Mesh::P_type P;

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    // ImGui options for color picker
    static const ImGuiColorEditFlags color_edit_flags = ImGuiColorEditFlags_NoSidePreview  |
                                                        ImGuiColorEditFlags_NoTooltip      |
                                                        ImGuiColorEditFlags_NoLabel        |
                                                        ImGuiColorEditFlags_NoInputs       |
                                                        ImGuiColorEditFlags_PickerHueWheel |
                                                        ImGuiColorEditFlags_NoBorder;

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    DrawableIsocontour<M,V,E,P> isocontour;
    DrawableVectorField         vec_field;

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    bool  show_mesh         = true;
    bool  show_wireframe    = true;
    bool  show_isoline      = false;
    bool  show_vecfield     = false;
    int   shading           = 2; // 0 = point, 1 = flat, 2 = smooth
    int   wireframe_width   = 1;
    float wireframe_alpha   = 1;
    int   isoline_width     = 1;
    float vecfield_size     = 0.9;
    float iso_val           = 0.5;
    float iso_min           = 0;
    float iso_max           = 1;
    int   rb_color          = 0;
    SlicerState slice;
    Color vert_color        = Color::WHITE();
    Color poly_color        = Color::WHITE();
    Color iso_color         = Color::RED();
    Color vec_color         = Color::RED();
    int   text_1d           = TEXTURE_1D_ISOLINES;
    int   text_2d           = TEXTURE_2D_ISOLINES;
    float text_scale_factor = 1.0;

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    public:

        SurfaceMeshControls(Mesh *m, GLcanvas *gui, const std::string & name);
       ~SurfaceMeshControls() override {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void draw() override;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // tematic collapsing headers
        void header_IO          (const bool show_open);
        void header_shading     (const bool show_open);
        void header_wireframe   (const bool show_open);
        void header_colors      (const bool show_open);
        void header_textures    (const bool show_open);
        void header_scalar_field(const bool show_open);
        void header_vector_field(const bool show_open);
        void header_AO          (const bool show_open);
        void header_slicing     (const bool show_open);
        void header_marked_edges(const bool show_open);
        void header_actions     (const bool show_open);
        void header_normals     (const bool show_open);
};

}

#ifndef  CINO_STATIC_LIB
#include "surface_mesh_controls.cpp"
#endif

#endif // CINO_SURFACE_MESH_CONTROLS_H
