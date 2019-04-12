/* This sample program computes the homotopy basis of an input mesh
 * using the method described in:
 *
 *   Greedy optimal homotopy and homology generators
 *   Jeff Erickson and Kim Whittlesey
 *   ACM-SIAM symposium on Discrete algorithms, 2005
 *
 * Enjoy!
*/

#include <QApplication>
#include <QGridLayout>
#include <QCheckBox>
#include <cinolib/gui/qt/qt_gui_tools.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/drawable_segment_soup.h>
#include <cinolib/homotopy_basis.h>
#include <cinolib/profiler.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication app(argc, argv);
    QWidget      window;
    QGridLayout  layout;
    GLcanvas     gui;
    QCheckBox    cb_tree  ("Tree", &window);
    QCheckBox    cb_cotree("Co-Tree", &window);
    QCheckBox    cb_basis ("Homotopy Basis", &window);
    cb_tree.setChecked(false);
    cb_cotree.setChecked(false);
    cb_basis.setChecked(true);
    layout.addWidget(&cb_tree,0,0);
    layout.addWidget(&cb_cotree,0,1);
    layout.addWidget(&cb_basis,0,2);
    layout.addWidget(&gui,1,0,1,3);
    window.setLayout(&layout);
    window.show();
    window.resize(800,600);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/torus.obj";
    DrawableTrimesh<> m(s.c_str());
    m.show_wireframe(false);
    gui.push_obj(&m);

    Profiler profiler;
    std::vector<std::vector<uint>> basis;
    std::vector<bool> tree, cotree;

    profiler.push("Homotopy basis");
    homotopy_basis(m, basis, tree, cotree);
    profiler.pop();

    // Visualization part

    DrawableSegmentSoup ss_basis;
    ss_basis.set_cheap_rendering(false);
    ss_basis.set_color(Color::BLACK());
    ss_basis.set_thickness(2);
    gui.push_obj(&ss_basis);

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
        ss_basis.push_seg(m.edge_vert(eid,0), m.edge_vert(eid,1));
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

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QCheckBox::connect(&cb_tree, &QCheckBox::stateChanged, [&]()
    {
        if(cb_tree.isChecked()) gui.push_obj(&ss_tree, false);
        else                    gui.pop(&ss_tree);
        gui.updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QCheckBox::connect(&cb_cotree, &QCheckBox::stateChanged, [&]()
    {
        if(cb_cotree.isChecked()) gui.push_obj(&ss_cotree, false);
        else                      gui.pop(&ss_cotree);
        gui.updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QCheckBox::connect(&cb_basis, &QCheckBox::stateChanged, [&]()
    {
        if(cb_basis.isChecked()) gui.push_obj(&ss_basis, false);
        else                     gui.pop(&ss_basis);
        gui.updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    SurfaceMeshControlPanel<DrawableTrimesh<>> panel(&m, &gui);
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_1), &gui), &QShortcut::activated, [&](){panel.show();});

    return app.exec();
}

