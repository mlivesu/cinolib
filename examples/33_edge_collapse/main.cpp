#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/glcanvas.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/circle.obj";
    DrawableTrimesh<> m(s.c_str());

    GLcanvas gui;
    gui.push(&m);

    std::cout << "\n\nPress SPACE to start the collapsing sequence\n\n" << std::endl;

    gui.callback_key_pressed = [&](int key, int) -> bool
    {
        if(key == GLFW_KEY_SPACE)
        {
            bool converged = false;
            do
            {
                converged = true;
                for(uint eid=0; eid<m.num_edges(); ++eid)
                {
                    if(m.edge_is_incident_to_boundary(eid)) continue;
                    if(m.edge_collapse(eid, 0.5, true, true))
                    {
                        converged = false;
                        m.updateGL();
                        gui.draw();
                    }
                }
            }
            while(!converged);
            m.updateGL();
        }
        return false;
    };

    gui.launch();
}
