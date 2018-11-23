/* This program takes as input a 2D polygon, and produces two outputs:
 *
 *   i)  a dense triangulation of such polygon, using the
 *       Triangle library (https://www.cs.cmu.edu/~quake/triangle.html)
 *
 *   ii) a dense polygonalization of the same polygon, defined
 *       as the dual of the previously generated triangle mesh
*/

#include <QApplication>
#include <QBoxLayout>
#include <cinolib/meshes/meshes.h>
#include <cinolib/dual_mesh.h>
#include <cinolib/triangle_wrap.h>
#include <cinolib/gui/qt/qt_gui_tools.h>
#include <cinolib/profiler.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication app(argc, argv);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/E.obj";
    DrawablePolygonmesh<> m_in(s.c_str());

    // make trimesh
    double area_thresh = 0.01 * m_in.bbox().diag(); // force triangles to be smaller than 5% of bbox diag
    char opt[100];
    sprintf(opt, "Qqa%f", area_thresh);
    DrawableTrimesh<> m_tri;
    triangle_wrap(m_in.vector_verts(), m_in.vector_edges(), {}, 0, opt, m_tri);
    m_tri.updateGL();

    Profiler profiler;

    // make polygon mesh
    DrawablePolygonmesh<> m_poly;
    profiler.push("Dualize mesh");
    dual_mesh(m_tri, m_poly, true);
    profiler.pop();
    m_poly.updateGL();

    QWidget  gui;
    QHBoxLayout layout;
    gui.setLayout(&layout);
    GLcanvas gui_in(&gui);
    GLcanvas gui_tri(&gui);
    GLcanvas gui_poly(&gui);
    layout.addWidget(&gui_in);
    layout.addWidget(&gui_tri);
    layout.addWidget(&gui_poly);
    gui_in.push_obj(&m_in);
    gui_tri.push_obj(&m_tri);
    gui_poly.push_obj(&m_poly);
    gui.show();
    gui.resize(600,400);
    m_in.show_wireframe(true);
    m_tri.show_wireframe(true);
    m_poly.show_wireframe(true);

    // CMD+1 to show in   mesh controls.
    // CMD+2 to show tri  mesh controls.
    // CMD+3 to show poly mesh controls.
    SurfaceMeshControlPanel<DrawablePolygonmesh<>> panel_in (&m_in,    &gui_in);
    SurfaceMeshControlPanel<DrawableTrimesh<>>     panel_tri(&m_tri,   &gui_tri);
    SurfaceMeshControlPanel<DrawablePolygonmesh<>> panel_poly(&m_poly, &gui_poly);
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_1), &gui_in),   &QShortcut::activated, [&](){panel_in.show();});
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_2), &gui_tri),  &QShortcut::activated, [&](){panel_tri.show();});
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_3), &gui_poly), &QShortcut::activated, [&](){panel_poly.show();});

    return app.exec();
}
