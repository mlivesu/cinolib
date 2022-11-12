#include <cinolib/meshes/meshes.h>
#include <cinolib/Poisson_sampling.h>
#include <cinolib/profiler.h>
#include <cinolib/gl/glcanvas.h>

int main(int, char **)
{
    using namespace cinolib;

    Profiler profiler;
    GLcanvas gui_2d;
    GLcanvas gui_3d;

    DrawableTrimesh<> m_2d;
    DrawableTrimesh<> m_3d;
    m_2d.show_mesh_points();
    m_3d.show_mesh_points();
    m_2d.show_wireframe_width(6);
    m_3d.show_wireframe_width(3);
    gui_2d.push(&m_2d);
    gui_3d.push(&m_3d);

    float radius = 0.02f;

    // update 2d and 3d Poisson sampling based on current parameters
    auto compute_samples = [&]()
    {
        m_2d.clear();
        m_3d.clear();

        std::vector<vec2d> samples_2d;
        profiler.push("Poisson sampling 2D");
        Poisson_sampling<2,vec2d>(radius, vec2d(0,0), vec2d(1,1), samples_2d);
        profiler.pop();
        for(auto p : samples_2d) m_2d.vert_add(vec3d(p.x(),p.y(),0));
        m_2d.vert_set_color(Color::BLACK());
        gui_2d.refit_scene();

        std::vector<vec3d> samples_3d;
        profiler.push("Poisson sampling 3D");
        Poisson_sampling<3,vec3d>(radius, vec3d(0,0,0), vec3d(1,1,1), samples_3d);
        profiler.pop();
        for(auto & p : samples_3d) m_3d.vert_add(p);
        m_3d.vert_set_color(Color::BLACK());
        gui_3d.refit_scene();
    };

    gui_2d.callback_app_controls = [&]()
    {
        ImGui::SliderFloat("Radius", &radius, 0.001f, 0.1f);
        if(ImGui::SmallButton("Compute Samples"))
        {
            compute_samples();
        }
    };

    compute_samples();

    return gui_2d.launch({&gui_3d});
}
