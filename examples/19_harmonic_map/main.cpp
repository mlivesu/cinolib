/* This sample program maps a disk-like triangulated surface
 * into the 2D unit circle. The map is computed by solving
 * the harmonic equation, fixing the boundary of the shape
 * on the perimeter of the circle through Dirichelet boundary
 * conditions. Alternative parametric domains could also be 
 * considered (e.g. a square or other convex polygons)
 *
 * Enjoy!
*/

#include <QApplication>
#include <QHBoxLayout>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gui/qt/qt_gui_tools.h>
#include <cinolib/harmonic_map.h>
#include <cinolib/geometry/n_sided_poygon.h>
#include <cinolib/gui/load_texture.h>
#include <cinolib/profiler.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication a(argc, argv);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/Laurana.obj";
    DrawableTrimesh<> m_xyz(s.c_str());

    // ordered list of boundary vertices
    std::vector<uint> boundary = m_xyz.get_ordered_boundary_vertices();

    // create parametric space (discrete unit circle with as many point as the boundary vertices)
    std::vector<vec3d> uv_boundary = n_sided_polygon(boundary.size(), CIRCLE);

    // set potitional constraints for boundary vertices (map mesh boundary to the unit circle)
    std::map<uint,vec3d> dirichlet_bcs;
    for(uint i=0; i<boundary.size(); ++i) dirichlet_bcs[boundary.at(i)] = uv_boundary.at(i);

    // solve for the interior vertices via harmonic map
    Profiler profiler;
    profiler.push("3D Harmonic map");
    std::vector<vec3d> uv_map = harmonic_map_3d(m_xyz, dirichlet_bcs);
    profiler.pop();

    // create a mesh of the uv parametric space (same connectivity, different embedding)
    DrawableTrimesh<> m_uvw(uv_map, m_xyz.vector_polys());

    // copy uv coordinates to m (for texture visualization)
    for(uint vid=0; vid<m_xyz.num_verts(); ++vid) m_xyz.vert_data(vid).uvw = m_uvw.vert(vid);

    // visualize original and parametric mesh in two separated windows
    QWidget window;
    GLcanvas gui_xyz;
    GLcanvas gui_uvw;
    QHBoxLayout layout;
    layout.addWidget(&gui_xyz);
    layout.addWidget(&gui_uvw);
    window.setLayout(&layout);
    m_xyz.show_wireframe(true);
    m_xyz.show_wireframe_transparency(0.4);
    m_xyz.show_texture2D(TEXTURE_2D_ISOLINES, 3.0);
    gui_xyz.push_obj(&m_xyz);
    m_uvw.show_wireframe(true);
    m_uvw.show_wireframe_transparency(0.4);
    m_uvw.show_texture2D(TEXTURE_2D_ISOLINES, 3.0);
    gui_uvw.push_obj(&m_uvw);
    window.resize(800,600);
    window.show();

    // CMD+1 to show XYZ mesh controls.
    // CMD+2 to show UVW mesh controls.
    SurfaceMeshControlPanel<DrawableTrimesh<>> panel_xyz(&m_xyz, &gui_xyz);
    SurfaceMeshControlPanel<DrawableTrimesh<>> panel_uvw(&m_uvw, &gui_uvw);
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_1), &gui_xyz), &QShortcut::activated, [&](){panel_xyz.show();});
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_2), &gui_uvw), &QShortcut::activated, [&](){panel_uvw.show();});

    return a.exec();
}
