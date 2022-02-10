#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/find_intersections.h>
#include <cinolib/profiler.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/two_spheres.obj";
    DrawableTrimesh<> m(s.c_str());

    Profiler p;
    p.push("Find intersections");
    std::set<ipair> inters;
    find_intersections(m, inters);
    p.pop();

    std::cout << inters.size() << " pairs of intersecting triangles were found (in red)" << std::endl;
    for(const auto & i : inters)
    {
        m.poly_data(i.first).color  = Color::RED();
        m.poly_data(i.second).color = Color::RED();
    }
    m.updateGL();

    GLcanvas gui;
    SurfaceMeshControls<DrawableTrimesh<>> menu(&m,&gui);
    gui.push(&m);
    gui.push(&menu);
    return gui.launch();
}
