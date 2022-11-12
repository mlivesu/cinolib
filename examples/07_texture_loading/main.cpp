#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string filename = (argc==3) ? std::string(argv[1]) : std::string(DATA_PATH) + "/blub_triangulated.obj";
    std::string texture  = (argc==3) ? std::string(argv[2]) : std::string(DATA_PATH) + "/blub_texture.png";

    GLcanvas gui_xyz, gui_uvw;

    DrawableTrimesh<> m_xyz(filename.c_str());
    m_xyz.show_texture2D(TEXTURE_2D_BITMAP, 1.f, texture.c_str());
    m_xyz.show_wireframe(false);
    gui_xyz.push(&m_xyz);

    DrawableTrimesh<> m_uvw = m_xyz;
    m_uvw.copy_uvw_to_xyz(UVW_param);
    m_uvw.update_normals();
    m_uvw.show_texture2D(TEXTURE_2D_BITMAP, 1.f, texture.c_str());
    m_uvw.show_wireframe(false);
    gui_uvw.push(&m_uvw);

    // only the main window can host visual controls, so all menus are pushed in gui_xyz
    SurfaceMeshControls<DrawableTrimesh<>> menu_xyz(&m_xyz, &gui_xyz, "object space");
    SurfaceMeshControls<DrawableTrimesh<>> menu_uvw(&m_uvw, &gui_xyz, "texture space");
    gui_xyz.push(&menu_xyz);
    gui_xyz.push(&menu_uvw);

    // the main window launches the event loop for itself and for all other windows in the app
    return gui_xyz.launch({&gui_uvw});
}
