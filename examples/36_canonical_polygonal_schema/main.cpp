/* This sample program computes a canonical polygonal schema of a closed manifodl
 * with genus g, which is basically a map to a regular polygon with 4g sides. For
 * more details refer to:
 *
 *    Obtaining a Canonical Polygonal Schema from a
 *    Greedy Homotopy Basis with Minimal Mesh Refinement
 *    Marco Livesu
 *    (submitted)
 *
 * and
 *
 *    Globally Optimal Surface Mapping for Surfaces with Arbitrary Topology
 *    Xin Li, Yunfan Bao, Xiaohu Guo, Miao Jin, Xianfeng Gu and Hong Qin
 *    IEEE Transactions on Visualization and Computer Graphics, 2008
 *
 * Enjoy!
*/

#include <QApplication>
#include <QHBoxLayout>
#include <cinolib/gui/qt/qt_gui_tools.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/homotopy_basis.h>
#include <cinolib/canonical_polygonal_schema.h>
#include <cinolib/polycube.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication app(argc, argv);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/torus.obj";
    DrawableTrimesh<> m_xyz(s.c_str());

    HomotopyBasisData data;
    data.globally_shortest = false;
    data.root = 152; // best for torus
    data.detach_loops = true;
    data.split_strategy = HYBRID_SPLIT_STRATEGY;
    homotopy_basis(m_xyz, data);
    std::cout << data << std::endl;
    DrawableTrimesh<> m_uvw;
    canonical_polygonal_schema(m_xyz, data, m_uvw);

    QWidget window;
    GLcanvas gui_xyz;
    GLcanvas gui_uvw;
    QHBoxLayout layout;
    layout.addWidget(&gui_xyz);
    layout.addWidget(&gui_uvw);
    window.setLayout(&layout);
    m_xyz.show_wireframe(false);
    m_xyz.edge_mark_boundaries();
    m_xyz.show_texture2D(TEXTURE_2D_CHECKERBOARD, 4);
    m_xyz.updateGL();
    gui_xyz.push_obj(&m_xyz);
    m_uvw.show_wireframe(false);
    m_uvw.edge_mark_boundaries();
    m_uvw.show_texture2D(TEXTURE_2D_CHECKERBOARD, 4);
    m_uvw.updateGL();
    gui_uvw.push_obj(&m_uvw);
    window.resize(800,600);
    window.show();

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    SurfaceMeshControlPanel<DrawableTrimesh<>> panel_xyz(&m_xyz, &gui_xyz);
    SurfaceMeshControlPanel<DrawableTrimesh<>> panel_uvw(&m_uvw, &gui_uvw);
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_1), &gui_xyz), &QShortcut::activated, [&](){panel_xyz.show();});
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_2), &gui_uvw), &QShortcut::activated, [&](){panel_uvw.show();});

    return app.exec();
}
