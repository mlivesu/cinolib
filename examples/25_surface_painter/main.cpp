#include <cinolib/gl/glcanvas.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/geodesics.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc>1) ? std::string(argv[1]) : std::string(DATA_PATH) + "/bunny.obj";
    DrawableTrimesh<> m(s.c_str());
    m.show_wireframe(false);
    m.show_vert_color();

    GeodesicsCache prefactored_matrices;
    compute_geodesics_amortized(m, prefactored_matrices, {0}); // init heat based solver
    float brush_size = 0.01;

    GLcanvas gui;
    gui.push(&m);
    gui.callback_app_controls = [&]()
    {
        ImGui::Text("Brush size");
        ImGui::SliderFloat("##size", &brush_size, 0.001, 0.5);
        if(ImGui::SmallButton("Reset"))
        {
            m.vert_set_color(Color::WHITE());
        }
    };

    // painting event (CMD + click)
    gui.callback_mouse_left_click = [&](int modifiers) -> bool
    {
        if(modifiers & GLFW_MOD_SUPER)
        {
            vec3d p;
            vec2d pos = gui.cursor_pos();
            if(gui.unproject(pos, p))
            {
                uint  vid        = m.pick_vert(p);
                ScalarField f    = compute_geodesics_amortized(m, prefactored_matrices, {vid});
                for(vid=0; vid<m.num_verts(); ++vid)
                {
                    float dist = 1.f - f[vid];
                    if(dist<=brush_size)
                    {
                        float val = m.vert_data(vid).color.g;
                        val -= (brush_size-dist)/brush_size;
                        if(val<0) val = 0.f;
                        m.vert_data(vid).color = Color(1,val,val);
                    }
                }
                m.updateGL();
            }
        }
        return false;
    };

    return gui.launch();
}
