#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/profiler.h>
#include <cinolib/remesh_BotschKobbelt2004.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/pyramid.off";
    cinolib::DrawableTrimesh<> m(s.c_str());
    m.show_marked_edge(true);
    m.show_wireframe(true);
    m.show_mesh_flat();

    float ang_thresh         = to_rad(60.0);
    float avg_edge_length    = m.edge_avg_length();
    float target_edge_length = avg_edge_length*0.1;
    int   n_iters            = 10;

    GLcanvas gui;
    gui.push(&m);
    gui.callback_app_controls = [&]()
    {
        if(ImGui::SmallButton("Mark Color Discontinuities"))
        {
            m.edge_mark_color_discontinuities();
            m.updateGL();
        }
        if(ImGui::SmallButton("Mark Boundaries"))
        {
            m.edge_mark_boundaries();
            m.updateGL();
        }
        ImGui::Text("Sharp Creases");
        ImGui::SliderAngle("##angle", &ang_thresh, 0);
        if(ImGui::SmallButton("Mark Creases"))
        {
            m.edge_mark_sharp_creases(ang_thresh);
            m.updateGL();
        }
        if(ImGui::SmallButton("Unmark all"))
        {
            m.edge_set_flag(MARKED,false);
            m.updateGL();
        }
        ImGui::Separator();
        ImGui::Text("N Iterations");
        ImGui::SliderInt("##iter", &n_iters, 1, 100);
        ImGui::Text("Target Edge Length");
        ImGui::SliderFloat("##target edge length", &target_edge_length, avg_edge_length*0.01, avg_edge_length*2);
        if(ImGui::SmallButton("Remesh"))
        {
            Profiler profiler;
            for(int i=0; i<n_iters; ++i)
            {
                profiler.push("Remesh iteration");
                remesh_Botsch_Kobbelt_2004(m, target_edge_length,true);
                profiler.pop();
                m.updateGL();
            }
        }
    };

    return gui.launch();
}
