#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>
#include <cinolib/profiler.h>
#include <cinolib/mean_curv_flow.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/bunny.obj";
    DrawableTrimesh<> m(s.c_str());

    GLcanvas gui;
    gui.push(&m);

    Profiler profiler;
    gui.callback_app_controls = [&]()
    {
        static int   n_iter        = 10;
        static float time_step     = 0.005;
        ImGui::InputInt("Iter.", &n_iter);
        ImGui::InputFloat("t", &time_step);
        if(ImGui::SmallButton("Map to Sphere"))
        {
            profiler.push("Compute MCF");
            MCF(m, (uint)n_iter, (double)time_step);
            profiler.pop();
            m.updateGL();
            gui.refit_scene();
        }
        if(ImGui::SmallButton("Reset"))
        {
            m = DrawableTrimesh<>(s.c_str());
            gui.refit_scene();
        }
    };

    SurfaceMeshControls<DrawableTrimesh<>> menu(&m, &gui);
    gui.push(&menu);

    return gui.launch();
}
