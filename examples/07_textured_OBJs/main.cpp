/* This program reads a textured 3D model from an OBJ file and visualizes it
 * in two separate GL canvases. On the first canvas the regular 3D shape is
 * shown with a texture applied on it (texture seams are shown in red). On the
 * second canvas the shape is shown flattened on the uv space.
 *
 * NOTE: if for some reason the texture is not automatically loaded, please
 * click on the "Load Text2D" button on the control panel, and select the file
 * "blub_texture.png". This should fix the problem.
 *
 * Enjoy!
*/

#include <QApplication>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gui/qt/glcanvas.h>

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
    std::string s = (argc==3) ? std::string(argv[1]) : std::string(DATA_PATH) + "/blub_triangulated.obj";
    read_OBJ(s.c_str(), xyz, uvw, poly);

    DrawableTrimesh<> m_xyz(xyz, poly);
    for(uint vid=0; vid<m_xyz.num_verts(); ++vid) m_xyz.vert_data(vid).uvw = uvw.at(vid);
    m_xyz.edge_mark_boundaries();

    DrawableTrimesh<> m_uvw = m_xyz;
    m_uvw.copy_uvw_to_xyz(UVW_param);
    m_uvw.update_normals();

    s = (argc==3) ? std::string(argv[2]) : std::string(DATA_PATH) + "/blub_texture.png";

    m_xyz.show_texture2D(TEXTURE_2D_BITMAP, 1.0, s.c_str());
    gui_xyz.push_obj(&m_xyz);

    m_uvw.show_texture2D(TEXTURE_2D_BITMAP, 1.0, s.c_str());
    gui_uvw.push_obj(&m_uvw);

    return a.exec();
}
