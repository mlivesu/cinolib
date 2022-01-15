#include <cinolib/meshes/meshes.h>
#include <cinolib/profiler.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/coarse_layout.h>
#include <cinolib/gl/surface_mesh_controls.h>
#include <cinolib/drawable_sphere.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/cubespikes.obj";
    DrawableQuadmesh<> m(s.c_str());

    Profiler profiler;
    profiler.push("coarse layout");
    compute_coarse_quad_layout(m);
    profiler.pop();
    m.poly_color_wrt_label();
    m.show_marked_edge_color(Color::BLACK());
    m.show_marked_edge_width(5);
    m.updateGL();

    GLcanvas gui;
    SurfaceMeshControls<DrawableQuadmesh<>> controls(&m, &gui);
    gui.push(&m);
    gui.push(&controls);

    float radius = m.bbox().delta * 0.02;
    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        if(m.vert_is_singular(vid)) gui.push(new DrawableSphere(m.vert(vid), radius));
    }

    return gui.launch();
}
