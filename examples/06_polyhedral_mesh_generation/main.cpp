/* This program takes as input a 3D shape, and produces two outputs:
 *
 *   i)  a dense tetrahedralization of such shape, using the
 *       Tetgen library (http://wias-berlin.de/software/tetgen/)
 *
 *   ii) a dense polyhedralization of the same shape, defined
 *       as the dual of the previously generated tetrahedral mesh
*/

#include <QApplication>
#include <QBoxLayout>
#include <cinolib/meshes/meshes.h>
#include <cinolib/dual_mesh.h>
#include <cinolib/tetgen_wrap.h>
#include <cinolib/gui/qt/glcanvas.h>
#include <cinolib/gui/qt/volume_mesh_control_panel.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication app(argc, argv);

    DrawableTrimesh<> m_in("../../data/3holes.obj");

    // make tetmesh
    std::vector<uint>   edges, tets;
    std::vector<double> verts;
    double vol_thresh = 0.01 * m_in.bbox().diag(); // force triangles to be smaller than 5% of bbox diag
    char opt[100];
    sprintf(opt, "YQqa%f", vol_thresh);
    tetgen_wrap(m_in.vector_coords(), m_in.vector_serialized_polys(), edges, opt, verts, tets);
    DrawableTetmesh<> tetmesh(verts, tets);

    // make polygon mesh
    DrawablePolyhedralmesh<> polymesh;
    dual_mesh(tetmesh, polymesh, true);
    polymesh.updateGL();
    polymesh.slicer_reset();

    QWidget  gui;
    QHBoxLayout layout;
    gui.setLayout(&layout);
    GLcanvas gui_in(&gui);
    GLcanvas gui_tet(&gui);
    GLcanvas gui_poly(&gui);
    layout.addWidget(&gui_in);
    layout.addWidget(&gui_tet);
    layout.addWidget(&gui_poly);
    gui_in.push_obj(&m_in);
    gui_tet.push_obj(&tetmesh);
    gui_poly.push_obj(&polymesh);
    gui.show();
    m_in.show_wireframe(true);

    // show interior
    m_in.slice(0.6, Z, LEQ, AND);
    tetmesh.slice(0.6, Z, LEQ, AND);
    polymesh.slice(0.6, Z, LEQ, AND);

    return app.exec();
}
