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
#include <cinolib/gui/qt/glcanvas.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication app(argc, argv);

    DrawablePolygonmesh<> m_in("../../data/E.obj");

    // make trimesh
    std::vector<std::vector<uint>> tris;
    std::vector<vec3d> holes, verts;
    double area_thresh = 0.01 * m_in.bbox().diag(); // force triangles to be smaller than 5% of bbox diag
    char opt[100];
    sprintf(opt, "Qqa%f", area_thresh);
    triangle_wrap(m_in.vector_verts(), m_in.vector_edges(), holes, opt, verts, tris);
    DrawableTrimesh<> trimesh(verts, tris);

    // make polygon mesh
    DrawablePolygonmesh<> polymesh;
    dual_mesh(trimesh, polymesh, true);
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
    m_in.show_wireframe(true);
    trimesh.show_wireframe(true);
    polymesh.show_wireframe(true);
    return app.exec();
}
