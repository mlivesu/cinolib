#include <fstream>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/profiler.h>
#include <cinolib/gl/file_dialog_save.h>
#include <cinolib/gl/surface_mesh_controls.h>

using namespace cinolib;

int main(int argc, char **argv)
{
    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/cube_minus_sphere.obj";
    DrawableTrimesh<> m(s.c_str());
    m.show_mesh_flat();
    m.show_marked_edge_width(4);

    float ang_thresh = to_rad(60);

    GLcanvas gui;
    SurfaceMeshControls<DrawableTrimesh<>> menu(&m, &gui);
    gui.push(&m);
    gui.push(&menu);

    gui.callback_app_controls = [&]()
    {
        ImGui::SliderAngle("##angle", &ang_thresh, 0);
        if(ImGui::SmallButton("Mark Creases"))
        {
            m.edge_mark_sharp_creases(ang_thresh);
            m.updateGL();
        }
        if(ImGui::SmallButton("Pad Creases"))
        {
            // split triangles to make sure that each triangles has at most one crease edge
            std::vector<uint> to_split;
            for(uint pid=0; pid<m.num_polys(); ++pid)
            {
                uint count=0;
                for(uint eid : m.adj_p2e(pid)) if(m.edge_data(eid).flags[MARKED]) ++count;
                if(count>1) to_split.push_back(pid);
            }
            SORT_VEC(to_split, true);
            for(uint pid : to_split) m.poly_split(pid, m.poly_centroid(pid));
            std::cout << "Padding sharp creases ("<< to_split.size() << " triangles were split)" << std::endl;
            m.updateGL();
        }
        if(ImGui::SmallButton("Export"))
        {
            std::string filename = file_dialog_save();
            if(!filename.empty())
            {
                m.save(filename.c_str());
                std::ofstream f;
                f.open(filename + "sharp_creases.txt");
                assert(f.is_open());
                for(uint eid=0; eid<m.num_edges(); ++eid)
                {
                    if(m.edge_data(eid).flags[MARKED]) f << m.edge_vert_id(eid,0) << " " << m.edge_vert_id(eid,1) << "\n";
                }
                f.close();
            }
        }
    };

    // use SHIFT + click to manually mark/unmark a crease edge
    gui.callback_mouse_left_click = [&](int modifier)
    {
        if(modifier & GLFW_MOD_SHIFT)
        {
            vec3d p;
            vec2d click = gui.cursor_pos();
            if(gui.unproject(click, p)) // transform click in a 3d point
            {
                uint eid = m.pick_edge(p);
                m.edge_data(eid).flags[MARKED] = !m.edge_data(eid).flags[MARKED];
                m.updateGL();
            }
        }
    };

    return gui.launch();
}
