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

struct Handle
{
    uint     vid;
    vec2d    click_2d;
    vec3d    click_3d;
    GLdouble zbuf;
};
std::vector<Handle> handles;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

std::vector<Handle>::iterator pick_handle(const vec3d & click_3d)
{
    return std::min_element(handles.begin(),
                            handles.end(),
                            [&](const Handle & a, const Handle & b)
                            {
                                return click_3d.dist(a.click_3d) < click_3d.dist(b.click_3d);
                            });
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char *argv[])
{
    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/bunny.obj";
    DrawableTrimesh<> m(s.c_str());
    m.show_vert_color();

    std::cout << "Usage:\n"
                 "  1) select one or more vertex handles with SHIFT + mouse click\n"
                 "  2) press SPACE to initialize the algorithm\n"
                 "  3) select and drag vertex handles with SHIFT + mouse click to prescribe a deformation\n"
                 "\n"
                 "\nNOTE: if the mesh has open boundaries, press B to constrain their position."
    << std::endl;

    GLcanvas gui;
    gui.push(&m);
    gui.push(new SurfaceMeshControls<DrawableTrimesh<>>(&m,&gui));

    ARAP_data data;
    std::vector<Handle>::iterator curr_handle = handles.end();

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
                    Handle h;
                    h.click_2d = click_2d;
                    h.click_3d = click_3d;
                    h.vid  = m.pick_vert(click_3d);
                    h.zbuf = gui.query_Z_buffer(click_2d);
                    std::cout << "defined handle at vert " << h.vid << std::endl;
                    handles.push_back(h);
                    data.bcs[h.vid] = m.vert(h.vid);
                    m.vert_data(h.vid).color = Color::RED();
                }
                else if(mode==MOV_HANDLES)
                {
                    curr_handle = pick_handle(click_3d);
                    std::cout << "selected handle " << curr_handle->vid << std::endl;
                    for(const auto & h : handles) m.vert_data(h.vid).color = Color::RED();
                    m.vert_data(curr_handle->vid).color = Color::YELLOW();
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
            std::cout << "DEFORMATION MODE: you can now move your handles with SHIFT + mouse drag" << std::endl;
            curr_handle = handles.begin(); // make sure curr_h is attached to a valid handle...
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
            gui.unproject(vec2d(x_pos,y_pos),curr_handle->zbuf, p);
            vec3d delta = p - curr_handle->click_3d;
            data.bcs[curr_handle->vid] += delta;
            curr_handle->click_3d = p;
            ARAP(m,data);
            m.vector_verts() = data.xyz_out;
            m.update_normals();
            m.updateGL();
            gui.draw();
            return true;
        }
        return false;
    };

    return gui.launch();
}
