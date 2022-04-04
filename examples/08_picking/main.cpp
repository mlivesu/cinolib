#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>
#include <cinolib/profiler.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/maxFace.obj";

    DrawableTrimesh<> m(s.c_str());
    m.show_vert_color();

    GLcanvas gui;
    SurfaceMeshControls<DrawableTrimesh<>> menu(&m, &gui);
    gui.push(&m);
    gui.push(&menu);

    // associates to the mouse single click event a function that
    // unprojects the 2D click point and looks for the mesh vertex
    // closest to it.
    // NOTE: the complexity here is linear w.r.t. the number of verts.
    // In my personal experience this has never been a bottleneck, not
    // even on big meshes. Should this be the case, accelleration data
    // structures (e.g. octrees) could be used
    Profiler profiler;
    gui.callback_mouse_left_click = [&](int modifiers) -> bool
    {
        if(modifiers & GLFW_MOD_SHIFT)
        {
            vec3d p;
            vec2d click = gui.cursor_pos();
            if(gui.unproject(click, p)) // transform click in a 3d point
            {
                profiler.push("Vertex pick");
                uint vid = m.pick_vert(p);
                profiler.pop();
                std::cout << "ID " << vid << std::endl;
                m.vert_data(vid).color = Color::RED();
                m.updateGL();
            }
        }
        return false;
    };

    return gui.launch();
}
