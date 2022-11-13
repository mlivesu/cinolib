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
    DrawableTrimesh<> obj(s.c_str());
    DrawableTrimesh<> cps;

    DrawableSegmentSoup obj_loops;
    DrawableSegmentSoup cps_edges;
    obj_loops.draw_joint_spheres = true;
    obj_loops.joint_sphere_subd = 2;
    cps_edges.draw_joint_spheres = true;
    cps_edges.joint_sphere_subd = 2;

    HomotopyBasisData data;
    data.globally_shortest = false;
    data.root = (argc==3) ? atoi(argv[2]) : 152; // best for torus
    data.detach_loops = true;
    data.split_strategy = EDGE_SPLIT_STRATEGY;

    auto CPS = [&]()
    {
        homotopy_basis(obj, data);
        canonical_polygonal_schema(obj, data, cps);
        std::cout << data << std::endl;
        // update UV
        auto xyz = obj.vector_verts();
        obj.vector_verts() = cps.vector_verts();
        obj.copy_xyz_to_uvw(UV_param);
        cps.copy_xyz_to_uvw(UV_param);
        obj.vector_verts() = xyz;
        // color code basis loops/CPS edges...
        obj_loops.clear();
        cps_edges.clear();
        obj_loops.colors.clear();
        cps_edges.colors.clear();
        for(uint eid=0; eid<obj.num_edges(); ++eid)
        {
            if(obj.edge_is_boundary(eid))
            {
                uint v0 = obj.edge_vert_id(eid,0);
                uint v1 = obj.edge_vert_id(eid,1);
                int loop_id = obj.vert_data(v0).label;
                if(loop_id<0) loop_id = obj.vert_data(v1).label;
                Color c = Color::scatter(uint(data.loops.size()),loop_id,1.f,1.f);
                obj_loops.push_seg(obj.vert(v0),obj.vert(v1),c);
                cps_edges.push_seg(cps.vert(v0),cps.vert(v1),c);
            }
        }
    };

    CPS();

    GLcanvas gui_obj;
    GLcanvas gui_cps;
    obj.show_texture2D(TEXTURE_2D_CHECKERBOARD, 1.f);
    obj.show_marked_edge(false);
    obj.show_wireframe(false);
    obj.updateGL();
    gui_obj.push(&obj);
    gui_obj.push(&obj_loops);
    cps.show_texture2D(TEXTURE_2D_CHECKERBOARD, 1.f);
    cps.show_marked_edge(false);
    cps.show_wireframe(false);
    cps.updateGL();
    gui_cps.push(&cps);
    gui_cps.push(&cps_edges);

    SurfaceMeshControls<DrawableTrimesh<>> menu_xyz(&obj,&gui_obj,"OBJ space");
    SurfaceMeshControls<DrawableTrimesh<>> menu_cps(&cps,&gui_obj,"CPS space");
    gui_obj.push(&menu_xyz);
    gui_obj.push(&menu_cps);

    gui_obj.callback_mouse_left_click = [&](int modifiers) -> bool
    {
        if(modifiers & GLFW_MOD_SHIFT)
        {
            vec3d p;
            vec2d click = gui_obj.cursor_pos();
            if(gui_obj.unproject(click, p)) // transform click in a 3d point
            {
                obj.load(s.c_str());
                data.globally_shortest = false;
                data.root = obj.pick_vert(p);
                std::cout << "Center Homotopy Basis at Vertex ID " << data.root << std::endl;
                CPS();
                obj.updateGL();
            }
        }
        return false;
    };

    // allow users to interactively edit cut thickness and color
    gui_obj.callback_app_controls = [&]()
    {
        ImGui::Text("Cut Thicknesses");
        ImGui::SliderFloat("#thickobj", &obj_loops.thickness,0,30);
        ImGui::SliderFloat("#thickcps", &cps_edges.thickness,0,30);
    };

    return gui_obj.launch({&gui_cps});
}
