#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/drawable_sliced_object.h>
#include <cinolib/gl/surface_mesh_controls.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc>1) ? std::string(argv[1]) : std::string(DATA_PATH) + "/T_supported.cli";
    double hatch  = (argc>2) ? atof(argv[2]) : 0.01;

    std::cout << "load " << s << std::endl;
    std::cout << "hatch is: " << hatch << std::endl;

    DrawableSlicedObj<> obj(s.c_str(), hatch);

    GLcanvas gui;
    SurfaceMeshControls<DrawableSlicedObj<>> menu(&obj, &gui);
    gui.push(&obj);
    gui.push(&menu);

    return gui.launch();
}
