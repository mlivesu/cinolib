#include <cinolib/meshes/meshes.h>
#include <cinolib/harmonic_map.h>
#include <cinolib/geometry/n_sided_poygon.h>
#include <cinolib/profiler.h>
#include <cinolib/gl/load_texture.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/Laurana.obj";
    DrawableTrimesh<> m_xyz(s.c_str());

    // ordered list of boundary vertices
    std::vector<uint> boundary = m_xyz.get_ordered_boundary_vertices();

    // create parametric space (discrete unit circle with as many point as the boundary vertices)
    std::vector<vec3d> uv_boundary = n_sided_polygon((uint)boundary.size(), CIRCLE);

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

    GLcanvas gui_xyz, gui_uvw;
    m_xyz.show_wireframe(true);
    m_xyz.show_wireframe_transparency(0.4f);
    m_xyz.show_texture2D(TEXTURE_2D_ISOLINES, 3.f);
    gui_xyz.push(&m_xyz);
    m_uvw.show_wireframe(true);
    m_uvw.show_wireframe_transparency(0.4f);
    m_uvw.show_texture2D(TEXTURE_2D_ISOLINES, 3.f);
    gui_uvw.push(&m_uvw);

    // only the main window can host visual controls, so all menus are pushed in gui_xyz
    SurfaceMeshControls<DrawableTrimesh<>> menu_xyz(&m_xyz, &gui_xyz, "object space");
    SurfaceMeshControls<DrawableTrimesh<>> menu_uvw(&m_uvw, &gui_xyz, "texture space");
    gui_xyz.push(&menu_xyz);
    gui_xyz.push(&menu_uvw);

    // the main window launches the event loop for itself and for all other windows in the app
    return gui_xyz.launch({&gui_uvw});
}
