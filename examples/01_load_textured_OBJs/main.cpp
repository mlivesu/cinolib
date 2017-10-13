#include <QApplication>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gui/qt/glcanvas.h>
#include <cinolib/gui/qt/surface_mesh_control_panel.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    GLcanvas gui_xyz, gui_uvw;
    gui_xyz.resize(800,600);
    gui_uvw.resize(400,300);
    gui_xyz.show();
    gui_uvw.show();

    std::vector<vec3d> xyz, uvw;
    std::vector<std::vector<uint>> poly;
    read_OBJ("../../data/blub_triangulated.obj", xyz, uvw, poly);

    DrawableTrimesh<> m_xyz(xyz, poly);

    for(uint vid=0; vid<m_xyz.num_verts(); ++vid) m_xyz.vert_data(vid).uvw    = uvw.at(vid);
    for(uint eid=0; eid<m_xyz.num_edges(); ++eid) m_xyz.edge_data(eid).marked = m_xyz.edge_is_boundary(eid);
    m_xyz.updateGL();

    DrawableTrimesh<> m_uvw = m_xyz;
    m_uvw.copy_uvw_to_xyz(UVW_param);
    m_uvw.update_normals();

    m_xyz.show_face_texture2D(TEXTURE_2D_BITMAP, 1.0, "../../data/blub_texture.png");
    m_uvw.show_face_texture2D(TEXTURE_2D_BITMAP, 1.0, "../../data/blub_texture.png");

    gui_xyz.push_obj(&m_xyz);
    gui_uvw.push_obj(&m_uvw);

    SurfaceMeshControlPanel<DrawableTrimesh<>> m_controls (&m_xyz, &gui_xyz);
    SurfaceMeshControlPanel<DrawableTrimesh<>> m_controls2(&m_uvw, &gui_uvw);
    m_controls.show();
    m_controls2.show();

    return a.exec();
}
