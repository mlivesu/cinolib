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
#include <cinolib/textures/textures.h>
#include <cinolib/profiler.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication a(argc, argv);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/Laurana.obj";
    DrawableTrimesh<> m(s.c_str());

    // ordered list of boundary vertices
    std::vector<uint> boundary = m.get_ordered_boundary_vertices();

    // create parametric space (discrete unit circle with as many point as the boundary vertices)
    std::vector<vec3d> uv_boundary = n_sided_polygon(vec3d(0,0,0), boundary.size(), 1.0);

    // set potitional constraints for boundary vertices (map mesh boundary to the unit circle)
    std::map<uint,vec3d> dirichlet_bcs;
    for(uint i=0; i<boundary.size(); ++i) dirichlet_bcs[boundary.at(i)] = uv_boundary.at(i);

    // solve for the interior vertices via harmonic map
    Profiler profiler;
    profiler.push("3D Harmonic map");
    std::vector<vec3d> uv_map = harmonic_map_3d(m, dirichlet_bcs);
    profiler.pop();

    // create a mesh of the uv parametric space (same connectivity, different embedding)
    DrawableTrimesh<> m_uv(uv_map, m.vector_polys());

    // copy uv coordinates to m (for texture visualization)
    for(uint vid=0; vid<m.num_verts(); ++vid) m.vert_data(vid).uvw = m_uv.vert(vid);

    // visualize original and parametric mesh in two separated windows
    QWidget window;
    GLcanvas gui;
    GLcanvas gui_uv;
    QHBoxLayout layout;
    layout.addWidget(&gui);
    layout.addWidget(&gui_uv);
    window.setLayout(&layout);
    m.show_wireframe(true);
    m.show_wireframe_transparency(0.6);
    m.show_texture2D(TEXTURE_2D_ISOLINES, 3.0);
    gui.push_obj(&m);
    m_uv.show_wireframe(true);
    m_uv.show_wireframe_transparency(0.6);
    m_uv.show_texture2D(TEXTURE_2D_ISOLINES, 3.0);
    gui_uv.push_obj(&m_uv);
    window.resize(800,600);
    window.show();

    return a.exec();
}
