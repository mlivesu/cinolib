#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/laplacian.h>
#include <cinolib/matrix_eigenfunctions.h>
#include <cinolib/profiler.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

Profiler p;
bool cot_w = true;          // use cotangent weights for laplacian
int  nf    = 50;            // number of eigenfunctions
int  curr_f=0;              // current eigenfucntion (for plotting)
std::vector<double> f;      // serialized eigenfucntion coefficients
std::vector<double> f_min;  // min coefficient for each eigenfucntion
std::vector<double> f_max;  // max coefficient for each eigenfucntion

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    std::string s = (argc>=2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/humanoid.off";
    DrawableTrimesh<> m(s.c_str());
    Eigen::SparseMatrix<double> L = laplacian(m, cot_w?COTANGENT:UNIFORM);

    GLcanvas gui;
    gui.push(&m);
    gui.push(new SurfaceMeshControls<DrawableTrimesh<>>(&m,&gui));

    auto eigs = [&]()
    {
        p.push("eigendecomposition");
        bool ok = matrix_eigenfunctions(L,true,nf,f,f_min,f_max);
        p.pop();
        assert(ok);
    };

    gui.callback_app_controls = [&]()
    {
        if(ImGui::InputInt("num f",&nf)) eigs();
        if(ImGui::SliderInt("f", &curr_f, 0, nf-1))
        {
            if(f.size()<nf*m.num_verts()) eigs();
            for(uint vid=0; vid<m.num_verts(); ++vid)
            {
                double val  = f.at(curr_f*m.num_verts()+vid);
                double norm = (val-f_min[curr_f])/(f_max[curr_f]-f_min[curr_f]);
                m.vert_data(vid).color = Color::red_white_blue_ramp_01(norm);
            }
            m.show_vert_color();
        }
        if(ImGui::Checkbox("Cotangent",&cot_w))
        {
            L = laplacian(m, cot_w?COTANGENT:UNIFORM);
            eigs();
        }
    };

    return gui.launch();
}
