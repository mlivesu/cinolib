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
#include <cinolib/gl/surface_mesh_controls.h>
#include <../external/imgui/imgui.h>
#include <cinolib/gl/file_dialog_open.h>
#include <cinolib/gl/file_dialog_save.h>
#include <iostream>

namespace cinolib
{

template <class Mesh>
CINO_INLINE
SurfaceMeshControls<Mesh>::SurfaceMeshControls(Mesh *m, GLcanvas *gui, const std::string & name)
    : VisualControl(name)
    , m(m)
    , gui(gui)
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Mesh>
CINO_INLINE
void SurfaceMeshControls<Mesh>::draw()
{
    header_IO          (false);
    header_shading     (false);
    header_wireframe   (false);
    header_colors      (false);
    header_textures    (false);
    header_scalar_field(false);
    header_vector_field(false);
    header_slicing     (false);
    header_marked_edges(false);
    header_normals     (false);
    header_AO          (false);
    header_actions     (false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Mesh>
CINO_INLINE
void SurfaceMeshControls<Mesh>::header_IO(const bool show_open)
{
    ImGui::SetNextItemOpen(show_open,ImGuiCond_Once);
    if(ImGui::CollapsingHeader("IO"))
    {
        if(ImGui::Button("Load"))
        {
            std::string filename = file_dialog_open();
            if(!filename.empty())
            {
                *m = Mesh(filename.c_str());
                gui->refit_scene();
            }
        }
        ImGui::SameLine();
        if(ImGui::Button("Save"))
        {
            std::string filename = file_dialog_save();
            if(!filename.empty())
            {
                m->save(filename.c_str());
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Mesh>
CINO_INLINE
void SurfaceMeshControls<Mesh>::header_shading(const bool show_open)
{
    ImGui::SetNextItemOpen(show_open,ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Shading"))
    {
        if(ImGui::Checkbox("Show mesh", &show_mesh))
        {
            if(m!=nullptr) m->show_mesh(show_mesh);
        }
        if(ImGui::RadioButton("Point ", &shading, 0)) m->show_mesh_points();
        if(ImGui::RadioButton("Flat  ", &shading, 1)) m->show_mesh_flat();
        if(ImGui::RadioButton("Smooth", &shading, 2)) m->show_mesh_smooth();
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Mesh>
CINO_INLINE
void SurfaceMeshControls<Mesh>::header_wireframe(const bool show_open)
{
    ImGui::SetNextItemOpen(show_open,ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Wireframe"))
    {
        if(ImGui::Checkbox   ("Show",         &show_wireframe        )) m->show_wireframe(show_wireframe);
        if(ImGui::SliderInt  ("Width",        &wireframe_width, 1, 10)) m->show_wireframe_width(wireframe_width);
        if(ImGui::SliderFloat("Transparency", &wireframe_alpha, 0,  1)) m->show_wireframe_transparency(wireframe_alpha);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Mesh>
CINO_INLINE
void SurfaceMeshControls<Mesh>::header_colors(const bool show_open)
{
    ImGui::SetNextItemOpen(show_open,ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Colors"))
    {
        if(ImGui::ColorEdit4("Vert Color", vert_color.rgba, color_edit_flags))
        {
            m->vert_set_color(vert_color);
            m->show_vert_color();
        }
        ImGui::SameLine();
        if(ImGui::Button("Vert"))
        {
            m->show_vert_color();
        }
        //
        if(ImGui::ColorEdit4("Poly Color", poly_color.rgba, color_edit_flags))
        {
            m->poly_set_color(poly_color);
            m->show_poly_color();
        }
        ImGui::SameLine();
        if(ImGui::Button("Poly"))
        {
            m->show_poly_color();
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Mesh>
CINO_INLINE
void SurfaceMeshControls<Mesh>::header_textures(const bool show_open)
{
    ImGui::SetNextItemOpen(show_open,ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Textures"))
    {
        if(ImGui::Button("Isolines"))
        {
            text_2d = TEXTURE_2D_ISOLINES;
            m->show_texture2D(text_2d,texture_scale_factor);
        }
        if(ImGui::Button("Checkerboard"))
        {
            text_2d = TEXTURE_2D_CHECKERBOARD;
            m->show_texture2D(text_2d,texture_scale_factor);
        }
        if(ImGui::Button("Load..."))
        {
            std::string filename = file_dialog_open();
            if(!filename.empty())
            {
                text_2d = TEXTURE_2D_BITMAP;
                m->show_texture2D(text_2d, texture_scale_factor, filename.c_str());
            }
        }
        if(ImGui::SliderFloat("Scaling", &texture_scale_factor, 0.01, 100))
        {
            if(text_2d==TEXTURE_2D_BITMAP) text_2d = TEXTURE_2D_ISOLINES;
            m->show_texture2D(text_2d,texture_scale_factor);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Mesh>
CINO_INLINE
void SurfaceMeshControls<Mesh>::header_scalar_field(const bool show_open)
{
    ImGui::SetNextItemOpen(show_open,ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Scalar Field"))
    {
        if(ImGui::Button("Isolines"))
        {
            text_1d = TEXTURE_1D_ISOLINES;
            m->show_texture1D(text_1d);
        }
        if(ImGui::Button("HSV"))
        {
            text_1d = TEXTURE_1D_HSV;
            m->show_texture1D(text_1d);
        }
        if(ImGui::Button("HSV + ISO"))
        {
            text_1d = TEXTURE_1D_HSV_W_ISOLINES;
            m->show_texture1D(text_1d);
        }
        if(ImGui::Button("Parula"))
        {
            text_1d = TEXTURE_1D_PARULA;
            m->show_texture1D(text_1d);
        }
        if(ImGui::Button("Parula + ISO"))
        {
            text_1d = TEXTURE_1D_PARULA_W_ISOLINES;
            m->show_texture1D(text_1d);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Mesh>
CINO_INLINE
void SurfaceMeshControls<Mesh>::header_vector_field(const bool show_open)
{
    ImGui::SetNextItemOpen(show_open,ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Vector Fields"))
    {
        ImGui::Text("ciao");
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Mesh>
CINO_INLINE
void SurfaceMeshControls<Mesh>::header_AO(const bool show_open)
{
    ImGui::SetNextItemOpen(show_open,ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Ambient Occlusion"))
    {
        ImGui::Text("ciao");
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Mesh>
CINO_INLINE
void SurfaceMeshControls<Mesh>::header_slicing(const bool show_open)
{
    ImGui::SetNextItemOpen(show_open,ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Slicing"))
    {
        ImGui::Text("ciao");
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Mesh>
CINO_INLINE
void SurfaceMeshControls<Mesh>::header_marked_edges(const bool show_open)
{
    ImGui::SetNextItemOpen(show_open,ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Marked Edges"))
    {
        ImGui::Text("ciao");
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Mesh>
CINO_INLINE
void SurfaceMeshControls<Mesh>::header_actions(const bool show_open)
{
    ImGui::SetNextItemOpen(show_open,ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Actions"))
    {
        ImGui::Text("ciao");
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Mesh>
CINO_INLINE
void SurfaceMeshControls<Mesh>::header_normals(const bool show_open)
{
    ImGui::SetNextItemOpen(show_open,ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Normals"))
    {
        ImGui::Text("ciao");
    }
}

}
