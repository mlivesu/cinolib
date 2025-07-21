#include <cinolib/meshes/meshes.h>
#include <cinolib/ARAP.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

enum
{
    ADD_HANDLES = 0,
    MOV_HANDLES = 1
};

int       mode = ADD_HANDLES;
ARAP_data data;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

uint pick_handle(const Trimesh<> & m,
                 const vec3d     & click_3d)
{
    return *std::min_element(data.handles.begin(), data.handles.end(),[&](const uint a, const uint b)
                             {
                                 return click_3d.dist(m.vert(a)) < click_3d.dist(m.vert(b));
                             });
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char *argv[])
{
    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/cactus.off";
    DrawableTrimesh<> m(s.c_str());


    GLcanvas gui;
    gui.push(&m);
    gui.push(new SurfaceMeshControls<DrawableTrimesh<>>(&m,&gui,"Deformed mesh"));
    gui.show_side_bar      = true;
    gui.depth_cull_markers = false;

    gui.callback_key_pressed = [&](int key, int mod) -> bool
    {
        if(key==GLFW_KEY_SPACE)
        {
            ARAP(m,data);
            return true;
        }
        return false;
    };

    gui.callback_app_controls = [&]()
    {
        if(ImGui::RadioButton("Add  handles",mode==ADD_HANDLES)) { mode = 0;                   }
        if(ImGui::RadioButton("Move handles",mode==MOV_HANDLES)) { mode = 1; data.init = true; }
        if(ImGui::Checkbox("Hard Constraints",&data.hard_constrain_handles))
        {
            data.init = true;
        }
    };

    GLdouble zbuf = 0;
    int curr_handle;
    gui.callback_mouse_left_click = [&](int mod) -> bool
    {
        if(mod & GLFW_MOD_SHIFT)
        {
            vec3d click_3d;
            vec2d click_2d = gui.cursor_pos();
            if(gui.unproject(click_2d, click_3d))
            {
                if(mode==ADD_HANDLES)
                {
                    uint vid  = m.pick_vert(click_3d);
                    data.handles.push_back(vid);
                    data.handles_x[vid] = m.vert(vid).x();
                    data.handles_y[vid] = m.vert(vid).y();
                    data.handles_z[vid] = m.vert(vid).z();
                    gui.push_marker(m.vert(vid), "", Color::BLUE(), 10);
                }
                else
                {
                    curr_handle = pick_handle(m, click_3d);
                    zbuf = gui.query_Z_buffer(click_2d);
                }
            }
            return true;
        }
        return false;
    };

    gui.callback_mouse_moved = [&](double x_pos, double y_pos) -> bool
    {
        if(mode==ADD_HANDLES) return false;

        bool left  = glfwGetMouseButton(gui.window,GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS;
        bool shift = glfwGetKey(gui.window,GLFW_KEY_LEFT_SHIFT )==GLFW_PRESS ||
                     glfwGetKey(gui.window,GLFW_KEY_RIGHT_SHIFT)==GLFW_PRESS;

        if(left && shift)
        {
            vec3d p;
            gui.unproject(vec2d(x_pos,y_pos),zbuf, p);
            vec3d delta = p - m.vert(curr_handle);
            data.handles_x.at(curr_handle) += delta.x();
            data.handles_y.at(curr_handle) += delta.y();
            data.handles_z.at(curr_handle) += delta.z();
            ARAP(m,data);
            m.updateGL();
            gui.pop_all_markers();
            for(uint vid : data.handles) gui.push_marker(m.vert(vid), "", Color::BLUE(), 10);
            gui.draw();
            return true;
        }
        return false;
    };
    return gui.launch();
}
