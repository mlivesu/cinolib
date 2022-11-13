#include <cinolib/meshes/meshes.h>
#include <cinolib/geodesics.h>
#include <cinolib/profiler.h>
#include <cinolib/drawable_isosurface.h>
#include <cinolib/gl/glcanvas.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    GLcanvas gui;

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/sphere.mesh";
    DrawableTetmesh<> m(s.c_str());
    compute_geodesics(m, {0, 10}, COTANGENT).copy_to_mesh(m); // generate some scalar field
    m.show_in_texture1D(TEXTURE_1D_HSV_W_ISOLINES);
    m.show_out_texture1D(TEXTURE_1D_HSV_W_ISOLINES);
    gui.push(&m);

    MeshSlicer ss;
    ss.Z_thresh = 0.5f;
    ss.slice(m);
    m.updateGL();

    float iso_val = 0.5f;
    DrawableIsosurface<> iso(m, iso_val);
    gui.push(&iso, false);

    Profiler profiler;
    gui.callback_app_controls = [&]()
    {
        if(ImGui::SliderFloat("iso", &iso_val, 0.f, 1.f))
        {
            iso = DrawableIsosurface<>(m,iso_val);
        }
        if(ImGui::SliderFloat("slice", &ss.Z_thresh, 0.f, 1.f))
        {
            ss.slice(m);
            m.updateGL();
        }
        if(ImGui::SmallButton("Tessellate"))
        {
            profiler.push("tessellate iso-surface");
            iso.tessellate(m);
            profiler.pop();
            ss.slice(m);
            m.updateGL();
        }
    };

    return gui.launch();
}
