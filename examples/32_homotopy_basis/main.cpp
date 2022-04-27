#include <cinolib/gl/glcanvas.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/drawable_segment_soup.h>
#include <cinolib/homotopy_basis.h>
#include <cinolib/profiler.h>

int main(int argc, char **argv)
{
    using namespace cinolib;


    std::string s = (argc>=2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/torus.obj";
    DrawableTrimesh<> m(s.c_str());
    m.show_wireframe(false);

    Profiler profiler;
    std::vector<std::vector<uint>> basis;
    std::vector<bool> tree, cotree;

    uint root = (argc==3) ? atoi(argv[2]) : 152; // best for torus
    profiler.push("Homotopy basis");
    double len = homotopy_basis(m, root, basis, tree, cotree);
    profiler.pop();
    std::cout << "Shortest homotopy basis centered at vertex " << root << ". Length is " << len << std::endl;

    // Visualization part
    DrawableSegmentSoup ss_basis;
    ss_basis.set_cheap_rendering(false);
    ss_basis.set_color(Color::BLACK());
    ss_basis.set_thickness(3);

    DrawableSegmentSoup ss_cotree;
    ss_cotree.set_cheap_rendering(true);
    ss_cotree.set_color(Color::BLUE());
    ss_cotree.set_thickness(3);

    DrawableSegmentSoup ss_tree;
    ss_tree.set_cheap_rendering(true);
    ss_tree.set_color(Color::RED());
    ss_tree.set_thickness(3);

    for(auto loop : basis)
    for(uint i=0; i<loop.size(); ++i)
    {
        uint v0  = loop.at(i);
        uint v1  = loop.at((i+1)%loop.size());
         int eid = m.edge_id(v0, v1);
        ss_basis.push_seg(m.edge_vert((uint)eid,0), m.edge_vert((uint)eid,1));
    }

    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        if(tree.at(eid))
        {
            ss_tree.push_seg(m.edge_vert(eid,0), m.edge_vert(eid,1));
        }
        else if(cotree.at(eid))
        {
            vec3d p    = m.edge_sample_at(eid, 0.5);
            auto  pids = m.adj_e2p(eid);
            assert(pids.size()==2);
            ss_cotree.push_seg(m.poly_centroid(pids[0]), p);
            ss_cotree.push_seg(m.poly_centroid(pids[1]), p);
        }
    }

    bool show_basis  = true;
    bool show_tree   = false;
    bool show_cotree = false;

    GLcanvas gui;
    gui.push(&m);
    gui.push(&ss_basis);
    gui.callback_app_controls = [&]()
    {
        if(ImGui::Checkbox("Basis", &show_basis))
        {
            if(show_basis) gui.push(&ss_basis, false);
            else           gui.pop(&ss_basis);
        }
        if(ImGui::Checkbox("Tree", &show_tree))
        {
            if(show_tree) gui.push(&ss_tree, false);
            else          gui.pop(&ss_tree);
        }
        if(ImGui::Checkbox("CoTree", &show_cotree))
        {
            if(show_cotree) gui.push(&ss_cotree, false);
            else            gui.pop(&ss_cotree);
        }
    };

    return gui.launch();
}

