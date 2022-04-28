#include <cinolib/gl/glcanvas.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/surface_mesh_controls.h>
#include <cinolib/connected_components.h>
#include <cinolib/export_cluster.h>
#include <cinolib/string_utilities.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char *argv[])
{
    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/Gravgen.obj";
    DrawableTrimesh<> m(s.c_str());

    std::vector<std::unordered_set<uint>> ccs;
    connected_components(m, ccs);

    uint n_ccs = ccs.size();
    std::cout << n_ccs << " connected components were found.\nPress Key S to save them on separate files" << std::endl;

    for(uint i=0; i<n_ccs; ++i)
    {
        Color c = Color::scatter(n_ccs,i);
        for(auto vid : ccs[i])
        {
            m.vert_data(vid).color = c;
        }
    }
    m.show_marked_edge(false);
    m.show_vert_color();
    m.updateGL();

    GLcanvas gui;
    gui.push(&m);
    gui.push(new SurfaceMeshControls<DrawableTrimesh<>>(&m,&gui));
    gui.callback_key_pressed = [&](int key, int mod) -> bool
    {
        if(key==GLFW_KEY_S)
        {
            auto basename = get_file_path(s,true);
            for(uint i=0; i<n_ccs; ++i)
            {
                for(uint vid : ccs[i])
                {
                    for(uint pid : m.adj_v2p(vid))
                    {
                        m.poly_data(pid).label = i;
                    }
                }
                Trimesh<> subm;
                export_cluster(m,i,subm);
                subm.save((basename + "_" + std::to_string(i) + ".obj").c_str());
            }
            return true;
        }
        return false;
    };

    return gui.launch();
}
