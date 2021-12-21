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
#include <cinolib/gradient.h>
#include <cinolib/scalar_field.h>
#include <iostream>
#include <sstream>

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
    if(m==nullptr || gui==nullptr) return;

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
        if(ImGui::SmallButton("Load"))
        {
            std::string filename = file_dialog_open();
            if(!filename.empty())
            {
                *m = Mesh(filename.c_str());
                gui->refit_scene();
            }
        }
        ImGui::SameLine();
        if(ImGui::SmallButton("Save"))
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
        if(ImGui::Checkbox("Show mesh", &show_mesh))  m->show_mesh(show_mesh);
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
        if(ImGui::SmallButton("Vert"))
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
        if(ImGui::SmallButton("Poly"))
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
        if(ImGui::SmallButton("Isolines"))
        {
            text_2d = TEXTURE_2D_ISOLINES;
            m->show_texture2D(text_2d,text_scale_factor);
        }
        if(ImGui::SmallButton("Checkerboard"))
        {
            text_2d = TEXTURE_2D_CHECKERBOARD;
            m->show_texture2D(text_2d,text_scale_factor);
        }
        if(ImGui::SmallButton("Load..."))
        {
            std::string filename = file_dialog_open();
            if(!filename.empty())
            {
                text_2d = TEXTURE_2D_BITMAP;
                m->show_texture2D(text_2d, text_scale_factor, filename.c_str());
            }
        }
        if(ImGui::SliderFloat("Scaling", &text_scale_factor, 0.01, 100))
        {
            if(text_2d==TEXTURE_2D_BITMAP) text_2d = TEXTURE_2D_ISOLINES;
            m->show_texture2D(text_2d,text_scale_factor);
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
        if(ImGui::SmallButton("Isolines"))
        {
            text_1d = TEXTURE_1D_ISOLINES;
            m->show_texture1D(text_1d);
        }
        if(ImGui::SmallButton("HSV"))
        {
            text_1d = TEXTURE_1D_HSV;
            m->show_texture1D(text_1d);
        }
        if(ImGui::SmallButton("HSV + ISO"))
        {
            text_1d = TEXTURE_1D_HSV_W_ISOLINES;
            m->show_texture1D(text_1d);
        }
        if(ImGui::SmallButton("Parula"))
        {
            text_1d = TEXTURE_1D_PARULA;
            m->show_texture1D(text_1d);
        }
        if(ImGui::SmallButton("Parula + ISO"))
        {
            text_1d = TEXTURE_1D_PARULA_W_ISOLINES;
            m->show_texture1D(text_1d);
        }

        auto update_isoline = [&]()
        {
            isocontour           = DrawableIsocontour<M,V,E,P>(*m,iso_val);
            isocontour.thickness = isoline_width;
            isocontour.color     = iso_color;
            gui->push(&isocontour,false);
        };
        if(ImGui::Checkbox("Isoline", &show_isoline))
        {
            if(show_isoline) update_isoline();
            else             gui->pop(&isocontour);
        }
        if(ImGui::SliderFloat("Val", &iso_val,iso_min,iso_max))
        {
            if(show_isoline)
            {
                gui->pop(&isocontour);
                update_isoline();
            }
        }
        if(ImGui::SmallButton("Update Range"))
        {
            iso_max = m->vert_max_uvw_value(U_param);
            iso_min = m->vert_min_uvw_value(U_param);
            iso_val = (iso_max-iso_min)*0.5;
        }
        if(ImGui::SliderInt("Width##1", &isoline_width, 1, 10))
        {
            if(show_isoline)
            {
                gui->pop(&isocontour);
                update_isoline();
            }
        }
        if(ImGui::ColorEdit4("Isoline Color", iso_color.rgba, color_edit_flags))
        {
            if(show_isoline)
            {
                gui->pop(&isocontour);
                update_isoline();
            }
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
        if(ImGui::Checkbox("Vector Field", &show_vecfield))
        {
            if(show_vecfield) gui->push(&vec_field,false);
            else              gui->pop(&vec_field);
        }
        if(ImGui::SmallButton("Gradient"))
        {
            vec_field = DrawableVectorField(*m);
            ScalarField f(m->serialize_uvw(U_param));
            vec_field = gradient_matrix(*m) * f;
            vec_field.normalize();
            vec_field.set_arrow_size(m->edge_avg_length()*vecfield_size);
            vec_field.set_arrow_color(vec_color);
        }
        if(ImGui::SliderFloat("Size", &vecfield_size, 0.1, 5))
        {
            if(show_vecfield)
            {
                vec_field.set_arrow_size(m->edge_avg_length()*vecfield_size);
            }
        }
        if(ImGui::ColorEdit4("Vec Color", vec_color.rgba, color_edit_flags))
        {
            if(show_vecfield)
            {
                vec_field.set_arrow_color(vec_color);
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Mesh>
CINO_INLINE
void SurfaceMeshControls<Mesh>::header_AO(const bool show_open)
{
    // STILL QT DEPENDENT!
    //ImGui::SetNextItemOpen(show_open,ImGuiCond_Once);
    //if(ImGui::CollapsingHeader("Ambient Occlusion"))
    //{
    //}
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Mesh>
CINO_INLINE
void SurfaceMeshControls<Mesh>::header_slicing(const bool show_open)
{
    ImGui::SetNextItemOpen(show_open,ImGuiCond_Once);
    if(ImGui::CollapsingHeader("Slicing"))
    {
        bool refresh = false;
        if(ImGui::SmallButton("Copy"))
        {
            glfwSetClipboardString(gui->window,slice.to_string().c_str());
        }
        ImGui::SameLine();
        if(ImGui::SmallButton("Paste"))
        {
            slice.from_string(glfwGetClipboardString(gui->window));
            refresh = true;
        }
        ImGui::SameLine();
        if(ImGui::SmallButton("Reset"))
        {
            slice.reset();
            refresh = true;
        }
        refresh |= ImGui::RadioButton("AND", (int*)&slice.mode_AND, 1); ImGui::SameLine();
        refresh |= ImGui::RadioButton("OR ", (int*)&slice.mode_AND, 0);
        refresh |= ImGui::SliderFloat("X",   &slice.X_thresh, 0, 1); ImGui::SameLine();
        refresh |= ImGui::Checkbox   ("##x", &slice.X_leq);
        refresh |= ImGui::SliderFloat("Y",   &slice.Y_thresh, 0, 1); ImGui::SameLine();
        refresh |= ImGui::Checkbox   ("##y", &slice.Y_leq);
        refresh |= ImGui::SliderFloat("Z",   &slice.Z_thresh, 0, 1); ImGui::SameLine();
        refresh |= ImGui::Checkbox   ("##z", &slice.Z_leq);
        refresh |= ImGui::SliderFloat("Q",   &slice.Q_thresh, 0, 1); ImGui::SameLine();
        refresh |= ImGui::Checkbox   ("##q", &slice.Q_leq);
        refresh |= ImGui::SliderInt  ("L",   &slice.L_filter, 0, 10); ImGui::SameLine();
        refresh |= ImGui::Checkbox   ("##l", &slice.L_is);
        if(refresh) m->slice(slice);
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
        if(ImGui::SliderInt("Width##2", &marked_edge_width, 0, 10))
        {
            m->show_marked_edge(marked_edge_width>0);
            m->show_marked_edge_width(marked_edge_width);
            m->updateGL();
        }
        if(ImGui::ColorEdit4("Color", marked_edge_color.rgba, color_edit_flags))
        {
            m->show_marked_edge_color(marked_edge_color);
            m->updateGL();
        }
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
        if(ImGui::Checkbox("Show Face Normals", &show_face_normals))
        {
            if(show_face_normals)
            {
                face_normals.clear();
                face_normals.set_cheap_rendering(true);
                face_normals.set_color(Color::BLUE());
                double l = gui->camera.scene_radius/5.0;
                for(uint pid=0; pid<m->num_polys(); ++pid)
                {
                    if(!m->poly_data(pid).flags[HIDDEN])
                    {
                        vec3d n = m->poly_data(pid).normal;
                        vec3d c = m->poly_centroid(pid);
                        face_normals.push_seg(c, c+(n*l));
                    }
                }
                gui->push(&face_normals,false);
            }
            else
            {
                gui->pop(&face_normals);
            }
        }
        if(ImGui::Checkbox("Show Vert Normals", &show_vert_normals))
        {
            if(show_vert_normals)
            {
                vert_normals.clear();
                vert_normals.set_cheap_rendering(true);
                vert_normals.set_color(Color::RED());
                double l = gui->camera.scene_radius/5.0;
                for(uint vid=0; vid<m->num_verts(); ++vid)
                {
                    if(m->vert_is_visible(vid))
                    {
                        vec3d n = m->vert_data(vid).normal;
                        vec3d p = m->vert(vid);
                        vert_normals.push_seg(p, p+(n*l));
                    }
                }
                gui->push(&vert_normals,false);
            }
            else
            {
                gui->pop(&vert_normals);
            }
        }
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
        bool refresh = false;
        if(ImGui::SmallButton("Unmark all edges"))
        {
            m->edge_set_flag(MARKED,false);
            refresh = true;
        }
        if(ImGui::SmallButton("Color wrt Label"))
        {
            m->poly_color_wrt_label();
            refresh = true;
        }
        if(ImGui::SmallButton("Label wrt Color"))
        {
            m->poly_label_wrt_color();
            refresh = true;
        }
        if(ImGui::SmallButton("Mark Color Discontinuities"))
        {
            m->edge_mark_color_discontinuities();
            refresh = true;
        }
        if(ImGui::SmallButton("Mark Boundaries"))
        {
            m->edge_mark_boundaries();
            refresh = true;
        }
        if(ImGui::SmallButton("Mark Creases"))
        {
            m->edge_mark_sharp_creases(to_rad(crease_deg));
            refresh = true;
        }
        ImGui::InputInt("Crease deg", &crease_deg);

        if(refresh) m->updateGL();
    }
}

}
