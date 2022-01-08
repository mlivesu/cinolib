#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/tetgen_wrap.h>
#include <cinolib/vector_serialization.h>
#include <cinolib/dual_mesh.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/3holes.obj";
    DrawableTrimesh<> m_in(s.c_str());
    m_in.rotate(vec3d(0,0,1), M_PI_2);
    m_in.rotate(vec3d(0,1,0), to_rad(15.0));
    m_in.update_bbox();

    // make tetmesh
    std::vector<uint>   edges, tets;
    std::vector<double> verts;
    double vol_thresh = 0.01 * m_in.bbox().diag(); // force tets to be smaller than 5% of bbox diag
    char opt[100];
    sprintf(opt, "YQqa%f", vol_thresh);
    tetgen_wrap(serialized_xyz_from_vec3d(m_in.vector_verts()), serialized_vids_from_polys(m_in.vector_polys()), edges, opt, verts, tets);
    DrawableTetmesh<> m_tet(verts, tets);
    m_tet.translate(vec3d(m_in.bbox().delta_x()*1.2,0,0));
    m_tet.update_bbox();

    // make polyhedral (voronoi) mesh
    DrawablePolyhedralmesh<> m_poly;
    dual_mesh(m_tet, m_poly, true);
    m_poly.translate(vec3d(m_in.bbox().delta_x()*1.2,0,0));
    m_poly.update_bbox();

    MeshSlicer slicer;
    slicer.Z_thresh = 0.6;
    slicer.slice(m_in);
    slicer.slice(m_tet);
    slicer.slice(m_poly);

    m_in.updateGL();
    m_tet.updateGL();
    m_poly.updateGL();

    GLcanvas gui;
    gui.push(&m_in);
    gui.push(&m_tet);
    gui.push(&m_poly);

    return gui.launch();
}
