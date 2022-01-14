#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>
#include <cinolib/sphere_coverage.h>
#include <cinolib/profiler.h>

int main(int, char **)
{
    using namespace cinolib;

    int n_points = 1000;
    std::vector<vec3d> samples;
    sphere_coverage(1000, samples);
    std::vector<uint> no_polys = {};
    DrawableTrimesh<> m(samples, no_polys);
    m.vert_set_color(Color::BLACK());
    m.show_wireframe_width(3);
    m.show_mesh_points();

    GLcanvas gui;
    gui.push(&m);

    Profiler profiler;
    gui.callback_app_controls = [&]()
    {
        ImGui::Text("Num Points:");
        if(ImGui::SliderInt("##Num points",&n_points,100,10000))
        {
            profiler.push("Sphere coverage");
            sphere_coverage((uint)n_points, samples);
            profiler.pop();
            m = DrawableTrimesh<>(samples, no_polys);
            m.vert_set_color(Color::BLACK());
            m.show_wireframe_width(3);
            m.show_mesh_points();
        }
    };

    return gui.launch();
}
