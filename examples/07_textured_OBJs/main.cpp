/* This program reads a textured 3D model from an OBJ file and visualizes it
 * in two separate GL canvases. On the first canvas the regular 3D shape is
 * shown with a texture applied on it (texture seams are shown in red). On the
 * second canvas the shape is shown flattened on the uv space.
 *
 * Enjoy!
*/

#include <QApplication>
#include <QHBoxLayout>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gui/qt/qt_gui_tools.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    QWidget window;
    GLcanvas gui_xyz, gui_uvw;
    QHBoxLayout layout;
    layout.addWidget(&gui_xyz);
    layout.addWidget(&gui_uvw);
    window.setLayout(&layout);
    window.resize(800,600);
    window.show();

    std::string s = (argc==3) ? std::string(argv[1]) : std::string(DATA_PATH) + "/blub_triangulated.obj";
    DrawableTrimesh<> m_xyz(s.c_str());
    m_xyz.show_wireframe(false);

    DrawableTrimesh<> m_uvw = m_xyz;
    m_uvw.copy_uvw_to_xyz(UVW_param);
    m_uvw.show_wireframe(false);
    m_uvw.update_normals();

    s = (argc==3) ? std::string(argv[2]) : std::string(DATA_PATH) + "/blub_texture.png";

    m_xyz.show_texture2D(TEXTURE_2D_BITMAP, 1.0, s.c_str());
    m_uvw.show_texture2D(TEXTURE_2D_BITMAP, 1.0, s.c_str());
    gui_xyz.push_obj(&m_xyz);
    gui_uvw.push_obj(&m_uvw);

    return a.exec();
}
