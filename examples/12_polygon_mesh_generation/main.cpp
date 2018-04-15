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
    std::vector<std::vector<uint>> tris;
    std::vector<vec3d> holes, verts;
    double area_thresh = 0.01 * m_in.bbox().diag(); // force triangles to be smaller than 5% of bbox diag
    char opt[100];
    sprintf(opt, "Qqa%f", area_thresh);
    triangle_wrap(m_in.vector_verts(), m_in.vector_edges(), holes, opt, verts, tris);
    DrawableTrimesh<> trimesh(verts, tris);

    Profiler profiler;

    // make polygon mesh
    DrawablePolygonmesh<> polymesh;
    profiler.push("Dualize mesh");
    dual_mesh(trimesh, polymesh, true);
    profiler.pop();
    polymesh.updateGL();

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
    gui_tri.push_obj(&trimesh);
    gui_poly.push_obj(&polymesh);
    gui.show();
    gui.resize(600,400);
    m_in.show_wireframe(true);
    trimesh.show_wireframe(true);
    polymesh.show_wireframe(true);
    return app.exec();
}
