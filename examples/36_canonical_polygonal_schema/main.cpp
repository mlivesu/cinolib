#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/homotopy_basis.h>
#include <cinolib/canonical_polygonal_schema.h>
#include <cinolib/polycube.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc>=2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/torus.obj";
    DrawableTrimesh<> m_xyz(s.c_str());
    DrawableTrimesh<> m_cps;
    m_xyz.show_wireframe(false);
    m_cps.show_wireframe(false);

    HomotopyBasisData data;
    data.globally_shortest = false;
    data.root = (argc==3) ? atoi(argv[2]) : 152; // best for torus
    data.detach_loops = true;
    data.split_strategy = EDGE_SPLIT_STRATEGY;

    auto CPS = [&]()
    {
        homotopy_basis(m_xyz, data);
        canonical_polygonal_schema(m_xyz, data, m_cps);
        std::cout << data << std::endl;
        m_xyz.edge_mark_boundaries();
        m_cps.edge_mark_boundaries();
        // set UV coordinates
        auto xyz = m_xyz.vector_verts();
        m_xyz.vector_verts() = m_cps.vector_verts();
        m_xyz.copy_xyz_to_uvw(UV_param);
        m_cps.copy_xyz_to_uvw(UV_param);
        m_xyz.vector_verts() = xyz;
    };

    CPS();

    GLcanvas gui_xyz;
    GLcanvas gui_uvw;
    m_xyz.show_texture2D(TEXTURE_2D_CHECKERBOARD, 1);
    m_xyz.updateGL();
    gui_xyz.push(&m_xyz);
    m_cps.show_texture2D(TEXTURE_2D_CHECKERBOARD, 1);
    m_cps.updateGL();
    gui_uvw.push(&m_cps);

    SurfaceMeshControls<DrawableTrimesh<>> menu_xyz(&m_xyz,&gui_xyz,"OBJ space");
    SurfaceMeshControls<DrawableTrimesh<>> menu_cps(&m_cps,&gui_xyz,"CPS space");
    gui_xyz.push(&menu_xyz);
    gui_xyz.push(&menu_cps);

    gui_xyz.callback_mouse_left_click = [&](int modifiers) -> bool
    {
        if(modifiers & GLFW_MOD_SHIFT)
        {
            vec3d p;
            vec2d click = gui_xyz.cursor_pos();
            if(gui_xyz.unproject(click, p)) // transform click in a 3d point
            {
                m_xyz.load(s.c_str());
                data.globally_shortest = false;
                data.root = m_xyz.pick_vert(p);
                std::cout << "Center Homotopy Basis at Vertex ID " << data.root << std::endl;
                CPS();
                m_xyz.updateGL();
            }
        }
        return false;
    };

    return gui_xyz.launch({&gui_uvw});
}
