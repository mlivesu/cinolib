#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/volume_mesh_controls.h>
#include <cinolib/profiler.h>
#include <cinolib/voxelize.h>
#include <cinolib/voxel_grid_to_hexmesh.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char *argv[])
{
    uint max_voxels_per_side = (argc>1) ? atoi(argv[1]) : 64;
    int  voxel_type = (argc>=2) ? atoi(argv[2]) : VOXEL_BOUNDARY | VOXEL_INSIDE;

    auto torus = [](const vec3d & p)
    {
        double R = 1;
        double r = 0.5;
        return pow(R - sqrt(p[0]*p[0] + p[1]*p[1]),2) + p[2]*p[2] - r*r;
    };

    VoxelGrid g;
    Profiler p;
    p.push("voxelize");
    voxelize(torus, AABB(vec3d(-2,-2,-2),vec3d(2,2,2)), max_voxels_per_side, g);
    p.pop();
    std::cout << "grid dimensions : " << g.dim[0] << " x " << g.dim[1] << " x " << g.dim[2] << std::endl;

    DrawableHexmesh<> h;
    p.push("voxel grid to hexmesh");
    voxel_grid_to_hexmesh(g,h,voxel_type);
    p.pop();
    h.poly_color_wrt_label();
    h.updateGL();

    GLcanvas gui;
    gui.push(&h);
    gui.push(new VolumeMeshControls<DrawableHexmesh<>>(&h,&gui,"voxel grid"));
    return gui.launch();
}
