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
#ifndef CINO_VOLUME_MESH_CONTROLS_H
#define CINO_VOLUME_MESH_CONTROLS_H

#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/side_bar_item.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/load_texture.h>
#include <cinolib/drawable_isosurface.h>
#include <cinolib/drawable_vector_field.h>
#include <cinolib/drawable_segment_soup.h>

namespace cinolib
{

template<class Mesh>
class VolumeMeshControls : public SideBarItem
{
    typedef typename Mesh::M_type M;
    typedef typename Mesh::V_type V;
    typedef typename Mesh::E_type E;
    typedef typename Mesh::F_type F;
    typedef typename Mesh::P_type P;

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    // choices for digging radio button
    enum { RESET, DIG, UNDIG, ISOLATE };

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    Mesh     * m   = nullptr;
    GLcanvas * gui = nullptr;

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    DrawableIsosurface<M,V,E,F,P> isosurface;
    DrawableVectorField           vec_field;
    DrawableSegmentSoup           face_normals;
    DrawableSegmentSoup           vert_normals;

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    bool       show_mesh         = true;
    bool       show_wireframe    = true;
    bool       show_marked_edges = true;
    bool       show_marked_faces = true;
    bool       show_isosurface   = false;
    bool       show_vecfield     = false;
    bool       show_face_normals = false;
    bool       show_vert_normals = false;
    int        wireframe_width   = 1;
    int        marked_edge_width = 1;
    int        isoline_width     = 1;
    float      wireframe_alpha   = 1;
    float      vecfield_size     = 0.9;
    float      iso_val           = 0.5;
    float      iso_min           = 0;
    float      iso_max           = 1;
    int        crease_deg        = 60;
    int        marker_font_size  = 10;
    int        marker_size       = 5;
    int        dig_choice        = RESET;
    bool       show_vert_ids     = false;
    bool       show_face_ids     = false;
    Color      vert_color        = Color::WHITE();
    Color      poly_color        = Color::WHITE();
    Color      vert_debug_color  = Color::RED();
    Color      face_debug_color  = Color::BLUE();
    Color      iso_color         = Color::RED();
    Color      vec_color         = Color::RED();
    Color      marked_edge_color = Color::RED();
    Color      marked_face_color = Color::BLUE();
    MeshSlicer slicer;

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    public:

        VolumeMeshControls(Mesh *m, GLcanvas *gui, const std::string & name = "");
       ~VolumeMeshControls() override {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void draw() override;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // tematic collapsing headers
        void header_IO                 (const bool open);
        void header_shading            (const bool open);
        void header_wireframe          (const bool open);
        void header_colors_textures_in (const bool open);
        void header_colors_textures_out(const bool open);
        void header_textures           (const bool open);
        void header_vector_field       (const bool open);
        void header_isosurface         (const bool open);
        void header_slicing            (const bool open);
        void header_marked_edges       (const bool open);
        void header_marked_faces       (const bool open);
        void header_ambient_occlusion  (const bool open);
        void header_manual_digging     (const bool open);
        void header_actions            (const bool open);
        void header_debug              (const bool open);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // ImGui options for color picker
        static const ImGuiColorEditFlags color_edit_flags = ImGuiColorEditFlags_NoSidePreview  |
                                                            ImGuiColorEditFlags_NoTooltip      |
                                                            ImGuiColorEditFlags_NoInputs       |
                                                            ImGuiColorEditFlags_PickerHueWheel |
                                                            ImGuiColorEditFlags_NoBorder;
};

}

#ifndef  CINO_STATIC_LIB
#include "volume_mesh_controls.cpp"
#endif

#endif // CINO_VOLUME_MESH_CONTROLS_H
