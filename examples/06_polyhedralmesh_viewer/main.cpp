#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/volume_mesh_controls.h>

int main(int argc, char **argv)
{
    using namespace cinolib;
    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/eight_voronoi.hedra";
    DrawablePolyhedralmesh<> m(s.c_str());
    GLcanvas gui;
    VolumeMeshControls<DrawablePolyhedralmesh<>> menu(&m, &gui);
    gui.push(&m);
    gui.push(&menu);
    return gui.launch();
}
