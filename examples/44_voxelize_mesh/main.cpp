#include <cinolib/profiler.h>
#include <cinolib/voxelize.h>
#include <cinolib/voxel_grid_to_hexmesh.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>
#include <cinolib/gl/volume_mesh_controls.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char *argv[])
{
    std::string s = (argc>=2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/lion_vase_poly.off";
    DrawablePolygonmesh<> m(s.c_str());

    uint max_voxels_per_side = (argc>=3) ? atoi(argv[2]) : 64;
    int  voxel_type = (argc>=4) ? atoi(argv[3]) : VOXEL_BOUNDARY | VOXEL_INSIDE;

    VoxelGrid g;
    Profiler p;
    p.push("voxelize");
    voxelize(m, max_voxels_per_side, g);
    p.pop();
    std::cout << "grid dimensions : " << g.dim[0] << " x " << g.dim[1] << " x " << g.dim[2] << std::endl;

    DrawableHexmesh<> h;
    p.push("voxel grid to hexmesh");
    voxel_grid_to_hexmesh(g,h,voxel_type);
    p.pop();
    h.poly_color_wrt_label();
    h.updateGL();

    GLcanvas gui;
    gui.push(&m);
    gui.push(&h);
    gui.push(new SurfaceMeshControls<DrawablePolygonmesh<>>(&m,&gui));
    gui.push(new VolumeMeshControls<DrawableHexmesh<>>(&h,&gui,"voxel grid"));
    return gui.launch();
}
