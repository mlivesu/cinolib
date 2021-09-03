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
#include <cinolib/gui/surface_mesh_controls.h>
#include <../external/imgui/imgui.h>
#include <cinolib/gui/file_dialog_open.h>
#include <cinolib/gui/file_dialog_save.h>
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
    // LOAD/SAVE mesh
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
            std::cout << "save to file: " << filename << std::endl;
            m->save(filename.c_str());
        }
    }

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    ImGui::Separator(); //::::::::::::::::::::::::::::::::::::::::::::::::::::
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    if(ImGui::Checkbox("Show mesh", &show_mesh))
    {
        if(m!=nullptr) m->show_mesh(show_mesh);
    }

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    ImGui::Separator(); //::::::::::::::::::::::::::::::::::::::::::::::::::::
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    // SHADING
    ImGui::Text("Shading");
    if(ImGui::RadioButton("Point ", &shading, 0)) m->show_mesh_points();
    if(ImGui::RadioButton("Flat  ", &shading, 1)) m->show_mesh_flat();
    if(ImGui::RadioButton("Smooth", &shading, 2)) m->show_mesh_smooth();

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    ImGui::Separator(); //::::::::::::::::::::::::::::::::::::::::::::::::::::
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    // WIREFRAME
    if(ImGui::Checkbox   ("Wireframe",    &show_wireframe        )) m->show_wireframe(show_wireframe);
    if(ImGui::SliderInt  ("Width",        &wireframe_width, 1, 10)) m->show_wireframe_width(wireframe_width);
    if(ImGui::SliderFloat("Transparency", &wireframe_alpha, 0,  1)) m->show_wireframe_transparency(wireframe_alpha);

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    ImGui::Separator(); //::::::::::::::::::::::::::::::::::::::::::::::::::::
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    ImGui::Text("Colors/Textures");
    ImGui::Button("Set"); ImGui::SameLine(); if(ImGui::RadioButton("Point  ", &color, 0)) {}
    ImGui::Button("Set"); ImGui::SameLine(); if(ImGui::RadioButton("Poly   ", &color, 1)) {}
    ImGui::Button("Set"); ImGui::SameLine(); if(ImGui::RadioButton("Text 1D", &color, 2)) {}
    ImGui::Button("Set"); ImGui::SameLine(); if(ImGui::RadioButton("Text 2D", &color, 3)) {}

    const char *combo_text1D_items[] = { "Lines", "Heat", "HSV" };
    static const char* item_current = combo_text1D_items[0];
    if(ImGui::BeginCombo("", item_current))
    {
        for(int n=0; n<3; ++n)
        {
            bool is_selected = (item_current == combo_text1D_items[n]);
            if(ImGui::Selectable(combo_text1D_items[n], is_selected))
            {
                item_current = combo_text1D_items[n];
            }
            if(is_selected) ImGui::SetItemDefaultFocus(); // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
        }
        ImGui::EndCombo();
    }
}

}
