#include <cinolib/meshes/meshes.h>
#include <cinolib/harmonic_map.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>
#include <cinolib/profiler.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/bunny.obj";
    DrawableTrimesh<> m(s.c_str());
    m.show_wireframe(false);
    GLcanvas gui;
    SurfaceMeshControls<DrawableTrimesh<>> menu(&m, &gui);
    gui.push(&m);
    gui.push(&menu);

    // prescribe boundary conditions
    // SHIFT + CLICK on a vertex to insert a maximum
    // CMD   + CLICK on a vertex to insert a minimum
    std::map<uint,double> dirichlet_bcs;
    bool has_at_least_one_min = false;
    bool has_at_least_one_max = false;
    gui.callback_mouse_left_click = [&](int modifiers) -> bool
    {
        vec3d p;
        vec2d click = gui.cursor_pos();
        if(gui.unproject(click, p))
        {
            uint vid = m.pick_vert(p);
            if(modifiers & GLFW_MOD_SHIFT)
            {
                dirichlet_bcs[vid] = 1.0;
                has_at_least_one_max = true;
                gui.push_marker(m.vert(vid),"", Color::RED());
            }
            else if(modifiers & GLFW_MOD_SUPER)
            {
                dirichlet_bcs[vid] = 0.0;
                has_at_least_one_min = true;
                gui.push_marker(m.vert(vid),"", Color::BLUE());

            }
        }
        return false;
    };

    // visual controls to setup the level of harmonicity (1,2,...)
    // and to compute and reset the field
    Profiler profiler;
    gui.callback_app_controls = [&]()
    {
        static int n_harmonic = 1;
        ImGui::InputInt("Harmonic n", &n_harmonic);
        if(ImGui::SmallButton("Compute Field"))
        {
            if(dirichlet_bcs.size()>=2 && has_at_least_one_max && has_at_least_one_min)
            {
                profiler.push("harmonic_map");
                harmonic_map(m, dirichlet_bcs, (uint)n_harmonic, COTANGENT).copy_to_mesh(m);
                profiler.pop();
                m.show_texture1D(TEXTURE_1D_PARULA_W_ISOLINES);
            }
        }
        if(ImGui::SmallButton("Reset"))
        {
            dirichlet_bcs.clear();
            has_at_least_one_max = false;
            has_at_least_one_min = false;
            m.show_poly_color();
            gui.pop_all_markers();
        }
    };

    return gui.launch();
}
