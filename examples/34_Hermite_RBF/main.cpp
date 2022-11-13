#include <cinolib/gl/glcanvas.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/tetgen_wrap.h>
#include <cinolib/RBF_Hermite.h>
#include <cinolib/RBF_kernels.h>
#include <cinolib/profiler.h>
#include <cinolib/drawable_isosurface.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/sphere_coarse.obj";

    // load, center the mesh in the origin, and scale it
    DrawableTrimesh<> srf_target(s.c_str());
    srf_target.center_bbox();
    srf_target.normalize_bbox();
    srf_target.updateGL();

    // tetrahedralize its (fattened) bounding box to evaluate the HRBF (1.5x)
    DrawableTetmesh<> vol_sampling;
    std::vector<uint> dummy;
    tetgen_wrap(srf_target.bbox().corners(1.5), srf_target.bbox().tris(), dummy, "qa0.00002", vol_sampling);

    // HRBF computatin,  using x^3 as RBF kernel
    Profiler profiler;
    profiler.push("Make HRBF");
    Hermite_RBF<CubicRBF> HRBF(srf_target.vector_verts(), srf_target.vector_vert_normals());
    profiler.pop();

    // evaluate the BRBF at each volume point
    profiler.push("Evaluate HRBF at volume sampling");
    ScalarField f = HRBF.eval(vol_sampling.vector_verts());
    f.copy_to_mesh(vol_sampling);
    profiler.pop();

    GLcanvas gui;
    gui.push(&vol_sampling);
    gui.push(&srf_target);

    // the BRHF interpolant is the zero level set of the so generated field
    DrawableIsosurface<> iso(vol_sampling, 0.f);
    gui.push(&iso);

    // slice the volume, hide the target surface (for visualization)
    MeshSlicer ss;
    ss.Z_thresh = 0.5;
    ss.slice(vol_sampling);
    vol_sampling.show_in_wireframe(false);
    vol_sampling.show_out_wireframe(false);
    vol_sampling.show_in_texture1D(TEXTURE_1D_PARULA_W_ISOLINES);
    vol_sampling.updateGL();
    //
    srf_target.show_wireframe(false);
    srf_target.show_mesh(false);
    srf_target.updateGL();

    return gui.launch();
}
