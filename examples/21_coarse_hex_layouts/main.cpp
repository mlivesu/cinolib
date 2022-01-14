#include <cinolib/meshes/meshes.h>
#include <cinolib/profiler.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/volume_mesh_controls.h>
#include <cinolib/coarse_layout.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/ellipsoid.mesh";
    DrawableHexmesh<> m(s.c_str());

    Profiler profiler;
    profiler.push("coarse layout");
    compute_coarse_hex_layout(m);
    profiler.pop();
    m.poly_color_wrt_label();
    m.updateGL();

    GLcanvas gui;
    VolumeMeshControls<DrawableHexmesh<>> menu(&m,&gui);
    gui.push(&m);
    gui.push(&menu);

    return gui.launch();
}
