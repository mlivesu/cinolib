#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/ambient_occlusion.h>
#include <cinolib/profiler.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/bamboo_pen.obj";
    DrawableQuadmesh<> m(s.c_str());

    GLcanvas gui;
    gui.push(&m);

    ambient_occlusion_srf_meshes(m);
    m.updateGL();

    gui.callback_app_controls = [&]()
    {
        if(ImGui::SliderFloat("AO", &m.AO_alpha, 0.f, 1.f))
        {
            m.updateGL();
        }
    };

    return gui.launch();
}
