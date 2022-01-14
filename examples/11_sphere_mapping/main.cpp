#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>
#include <cinolib/profiler.h>
#include <cinolib/mean_curv_flow.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/bunny.obj";
    DrawableTrimesh<> m_xyz(s.c_str());
    DrawableTrimesh<> m_uvw = m_xyz;

    GLcanvas gui_xyz, gui_uvw;
    gui_xyz.push(&m_xyz);
    gui_uvw.push(&m_uvw);

    Profiler profiler;
    gui_xyz.callback_app_controls = [&]()
    {
        static int   n_iter        = 10;
        static float time_step     = 0.005;
        ImGui::InputInt("Iter.", &n_iter);
        ImGui::InputFloat("t", &time_step);
        if(ImGui::SmallButton("Map to Sphere"))
        {
            profiler.push("Compute MCF");
            MCF(m_uvw, (uint)n_iter, (double)time_step);
            profiler.pop();
            m_uvw.updateGL();
            gui_uvw.refit_scene();
        }
        if(ImGui::SmallButton("Reset"))
        {
            m_uvw = m_xyz;
            m_uvw.updateGL();
            gui_uvw.refit_scene();
        }
    };

    // only the main window can host visual controls, so all menus are pushed in gui_xyz
    SurfaceMeshControls<DrawableTrimesh<>> menu_xyz(&m_xyz, &gui_xyz, "object space");
    SurfaceMeshControls<DrawableTrimesh<>> menu_uvw(&m_uvw, &gui_xyz, "sphere map");
    gui_xyz.push(&menu_xyz);
    gui_xyz.push(&menu_uvw);

    // the main window launches the event loop for itself and for all other windows in the app
    return gui_xyz.launch({&gui_uvw});
}
