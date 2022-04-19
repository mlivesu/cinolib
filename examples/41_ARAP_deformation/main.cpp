#include <cinolib/gl/glcanvas.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/surface_mesh_controls.h>
#include <cinolib/ARAP.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

enum
{
    // the program starts in this phase. Users can click on the mesh to
    // select deformation handles. Each handle constraints a vertex to its
    // current position, and can be interactively moved by the user to
    // impose the wanted deformation
    DEF_HANDLES,

    // allows to reposition handles in the scene to prescribe a deformation
    MOV_HANDLES
};
int mode = DEF_HANDLES;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

std::vector<uint> handles;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

uint pick_handle(const Trimesh<> & m,
                 const vec3d     & click_3d)
{
    return *std::min_element(handles.begin(), handles.end(), [&](const uint a, const uint b)
                             {
                                 return click_3d.dist(m.vert(a)) < click_3d.dist(m.vert(b));
                             });
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char *argv[])
{
    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/cactus.off";
    DrawableTrimesh<> m(s.c_str());
    m.show_vert_color();

    std::cout << "Usage:\n"
                 "  1) select one or more vertex handles with SHIFT + mouse click\n"
                 "  2) press SPACE to initialize the algorithm\n"
                 "  3) select and drag vertex handles with SHIFT + mouse click to prescribe a deformation\n"
                 "\n"
                 "\nNOTE: if the mesh has open boundaries, press B to constrain their position."
    << std::endl;

    ARAP_data data;
    int curr_handle;

    GLcanvas gui;
    gui.push(&m);
    gui.push(new SurfaceMeshControls<DrawableTrimesh<>>(&m,&gui));
    gui.depth_cull_markers = false;

    GLdouble zbuf = 0;
    gui.callback_mouse_left_click = [&](int mod) -> bool
    {
        if(mod & GLFW_MOD_SHIFT)
        {
            vec3d click_3d;
            vec2d click_2d = gui.cursor_pos();
            if(gui.unproject(click_2d, click_3d))
            {
                if(mode==DEF_HANDLES)
                {
                    uint vid  = m.pick_vert(click_3d);
                    handles.push_back(vid);
                    data.bcs[vid] = m.vert(vid);
                    std::cout << "defined handle at vert " << vid << std::endl;
                    gui.push_marker(m.vert(vid), "", Color::BLUE(), 10);
                }
                else if(mode==MOV_HANDLES)
                {
                    curr_handle = pick_handle(m, click_3d);
                    zbuf = gui.query_Z_buffer(click_2d);
                    std::cout << "selected handle " << curr_handle << std::endl;
                }
            }
            m.updateGL();
            return true;
        }
        return false;
    };

    gui.callback_key_pressed = [&](int key, int modifiers) -> bool
    {
        if(key==GLFW_KEY_SPACE && !handles.empty())
        {
            mode = MOV_HANDLES;
            std::cout << "\nDEFORMATION MODE: you can now move your handles with SHIFT + mouse drag\n" << std::endl;
            curr_handle = *handles.begin(); // make sure curr_h is attached to a valid handle...
        }
        else if(key==GLFW_KEY_B)
        {
            auto boundary = m.get_boundary_vertices();
            for(uint vid : boundary) data.bcs[vid] = m.vert(vid);
            std::cout << "Constrained " << boundary.size() << " boundary vertices" << std::endl;
        }
        return false;
    };

    gui.callback_mouse_moved = [&](double x_pos, double y_pos) -> bool
    {
        if(mode==DEF_HANDLES) return false;

        bool left  = glfwGetMouseButton(gui.window,GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS;
        bool shift = glfwGetKey(gui.window,GLFW_KEY_LEFT_SHIFT )==GLFW_PRESS ||
                     glfwGetKey(gui.window,GLFW_KEY_RIGHT_SHIFT)==GLFW_PRESS;

        if(left && shift)
        {
            vec3d p;
            gui.unproject(vec2d(x_pos,y_pos),zbuf, p);
            vec3d delta = p - m.vert(curr_handle);
            data.bcs[curr_handle] += delta;
            ARAP(m,data);
            m.updateGL();
            gui.pop_all_markers();
            for(uint vid : handles) gui.push_marker(m.vert(vid), "", Color::BLUE(), 10);
            gui.draw();
            return true;
        }
        return false;
    };

    return gui.launch();
}
