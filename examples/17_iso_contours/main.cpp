#include <cinolib/meshes/meshes.h>
#include <cinolib/geodesics.h>
#include <cinolib/profiler.h>
#include <cinolib/drawable_isocontour.h>
#include <cinolib/gl/glcanvas.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    GLcanvas gui;

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/circle.obj";
    DrawableTrimesh<> m(s.c_str());
    compute_geodesics(m, {100}, COTANGENT).copy_to_mesh(m); // generate some scalar field
    m.show_texture1D(TEXTURE_1D_HSV_W_ISOLINES);
    m.show_wireframe(true);
    m.show_wireframe_transparency(0.5f);
    gui.push(&m);

    float iso_val = 0.5f;
    DrawableIsocontour<> iso(m, iso_val);
    iso.thickness = 3.f;
    gui.push(&iso, false);

    Profiler profiler;
    gui.callback_app_controls = [&]()
    {
        if(ImGui::SliderFloat("iso", &iso_val, 0.0, 1.0))
        {
            iso = DrawableIsocontour<>(m,iso_val);
            iso.thickness = 3.0;
        }
        if(ImGui::SmallButton("Tessellate"))
        {
            profiler.push("tessellate iso-contour");
            iso.tessellate(m);
            profiler.pop();
            m.updateGL();
        }
    };

    return gui.launch();
}
