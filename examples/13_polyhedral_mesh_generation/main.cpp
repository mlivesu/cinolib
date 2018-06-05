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
#include <cinolib/gui/qt/qt_gui_tools.h>
#include <cinolib/vector_serialization.h>
#include <cinolib/profiler.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication app(argc, argv);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/3holes.obj";
    DrawableTrimesh<> m_in(s.c_str());

    // make tetmesh
    std::vector<uint>   edges, tets;
    std::vector<double> verts;
    double vol_thresh = 0.01 * m_in.bbox().diag(); // force tets to be smaller than 5% of bbox diag
    char opt[100];
    sprintf(opt, "YQqa%f", vol_thresh);
    tetgen_wrap(serialized_xyz_from_vec3d(m_in.vector_verts()), serialized_vids_from_polys(m_in.vector_polys()), edges, opt, verts, tets);
    DrawableTetmesh<> tetmesh(verts, tets);

    Profiler profiler;

    // make polygon mesh
    DrawablePolyhedralmesh<> polymesh;
    profiler.push("Dualize mesh");
    dual_mesh(tetmesh, polymesh, true);
    profiler.pop();
    polymesh.updateGL();

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
    gui.resize(800,600);
    m_in.show_wireframe(true);

    // show interior
    SlicerState slice_params;
    slice_params.Z_thresh = 0.6;
    m_in.slice(slice_params);
    tetmesh.slice(slice_params);
    polymesh.slice(slice_params);

    return app.exec();
}
