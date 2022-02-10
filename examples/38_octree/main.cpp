#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/drawable_octree.h>
#include <cinolib/drawable_segment_soup.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc>=2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/bunny.obj";
    DrawableTrimesh<> m(s.c_str());
    m.show_wireframe(false);

    uint max_depth      = (argc>=3) ? atoi(argv[2]) :  7;
    uint items_per_leaf = (argc>=4) ? atoi(argv[3]) : 50;

    DrawableOctree octree(max_depth, items_per_leaf);
    octree.debug_mode(true); // dump times and statistics
    octree.build_from_mesh_polys(m);
    octree.updateGL();

    DrawableSegmentSoup ss;
    ss.set_color(Color::BLUE());
    ss.set_cheap_rendering(true);
    ss.set_thickness(5);

    GLcanvas gui;
    SurfaceMeshControls<DrawableTrimesh<>> menu(&m,&gui);
    gui.push(&m);
    gui.push(&menu);
    gui.push(&ss);

    gui.callback_mouse_left_click = [&](int modifiers)
    {
        if(modifiers & GLFW_MOD_SHIFT)
        {
            vec3d p;
            vec2d click = gui.cursor_pos();
            if(gui.unproject(click, p)) // transform click in a 3d point
            {
                double t;
                uint   pid;
                vec3d  dir = m.centroid() - p;
                if(octree.intersects_ray(p-dir, dir, t, pid)) // consider only the first hit
                {
                    std::cout << "hit triangle " << pid << std::endl;
                    m.poly_data(pid).color = Color::RED();
                    ss.push_seg(p-dir,m.centroid());
                    m.updateGL();
                }
            }
        }
    };

    bool show_octree = false;
    gui.callback_app_controls = [&]()
    {
        if(ImGui::Checkbox("Show Octree", &show_octree))
        {
            if(show_octree) gui.push(&octree,false);
            else            gui.pop (&octree);
        }
    };

    return gui.launch();
}
