#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/homotopy_basis.h>
#include <cinolib/canonical_polygonal_schema.h>
#include <cinolib/polycube.h>
#include <cinolib/drawable_segment_soup.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc>=2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/torus.obj";
    DrawableTrimesh<> m_xyz(s.c_str());
    DrawableTrimesh<> m_cps;

    DrawableSegmentSoup cuts_xyz;
    DrawableSegmentSoup cuts_cps;
    cuts_xyz.draw_joint_speheres = true;
    cuts_xyz.joint_sphere_subd = 2;
    cuts_cps.draw_joint_speheres = true;
    cuts_cps.joint_sphere_subd = 2;

    HomotopyBasisData data;
    data.globally_shortest = false;
    data.root = (argc==3) ? atoi(argv[2]) : 152; // best for torus
    data.detach_loops = true;
    data.split_strategy = EDGE_SPLIT_STRATEGY;

    auto update_cuts = [](DrawableTrimesh<> & m, DrawableSegmentSoup & ss)
    {
        ss.clear();
        for(uint eid=0; eid<m.num_edges(); ++eid)
        {
            if(m.edge_is_boundary(eid))
            {
                ss.push_seg(m.edge_vert(eid,0), m.edge_vert(eid,1));
            }
        }
    };

    auto CPS = [&]()
    {
        homotopy_basis(m_xyz, data);
        canonical_polygonal_schema(m_xyz, data, m_cps);
        std::cout << data << std::endl;
        update_cuts(m_xyz,cuts_xyz);
        update_cuts(m_cps,cuts_cps);
        // update UV
        auto xyz = m_xyz.vector_verts();
        m_xyz.vector_verts() = m_cps.vector_verts();
        m_xyz.copy_xyz_to_uvw(UV_param);
        m_cps.copy_xyz_to_uvw(UV_param);
        m_xyz.vector_verts() = xyz;        
    };

    CPS();

    GLcanvas gui_xyz;
    GLcanvas gui_cps;
    m_xyz.show_texture2D(TEXTURE_2D_CHECKERBOARD, 1);
    m_xyz.show_wireframe(false);
    m_xyz.updateGL();
    gui_xyz.push(&m_xyz);
    gui_xyz.push(&cuts_xyz);
    m_cps.show_texture2D(TEXTURE_2D_CHECKERBOARD, 1);
    m_cps.show_wireframe(false);
    m_cps.updateGL();
    gui_cps.push(&m_cps);
    gui_cps.push(&cuts_cps);

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

    // allow users to interactively edit cut thickness and color
    gui_xyz.callback_app_controls = [&]()
    {
        ImGui::Text("Cut Thicknesses");
        ImGui::SliderFloat("#thickobj", &cuts_xyz.thickness,0,30);
        ImGui::SliderFloat("#thickcps", &cuts_cps.thickness,0,30);
        ImGui::Text("Cut Colors");
        ImGui::ColorPicker4("#colobj", cuts_xyz.default_color.rgba, menu_xyz.color_edit_flags);
        ImGui::ColorPicker4("#colcps", cuts_cps.default_color.rgba, menu_xyz.color_edit_flags);
    };

    return gui_xyz.launch({&gui_cps});
}
