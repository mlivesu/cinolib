#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>
#include <cinolib/AFM/AFM.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

AFM_data data;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void stats(const char *filename = NULL)
{
    data.tris_out     = data.m0.num_polys();
    data.mesh_growth  = (data.tris_out-data.tris_in)/float(data.tris_in);
    data.converged    = (data.m0.vert_valence(data.origin)==data.m1.vert_valence(data.origin));
    data.flips_exact  = count_flipped(data,true);
    data.flips_double = count_flipped(data,false);

    int domain;
    std::string domain_str;
    switch(data.target_domain)
    {
        case CIRCLE : domain = 0; domain_str = "CIRCLE"; break;
        case SQUARE : domain = 1; domain_str = "SQUARE"; break;
        case STAR   : domain = 2; domain_str = "STAR";   break;
    }

    std::cout << ":::::::::::::::: STATS ::::::::::::::::"                  << std::endl;
    std::cout << "        Target domain: " << domain_str                    << std::endl;
    std::cout << "---------------------------------------"                  << std::endl;
    std::cout << "   Refinement enabled: " << data.refinement_enabled       << std::endl;
    std::cout << "    Abort if too slow: " << data.abort_if_too_slow        << std::endl;
    std::cout << "    Max time per step: " << data.max_time_per_step << "s" << std::endl;
    std::cout << "---------------------------------------"                  << std::endl;
    std::cout << "              Tris in: " << data.tris_in                  << std::endl;
    std::cout << "             Tris out: " << data.tris_out                 << std::endl;
    std::cout << "          Mesh growth: " << data.mesh_growth << "%"       << std::endl;
    std::cout << "---------------------------------------"                  << std::endl;
    std::cout << "            Moves TOT: " << data.moves_tot                << std::endl;
    std::cout << "                Flips: " << data.moves_flip               << std::endl;
    std::cout << "               Splits: " << data.moves_split              << std::endl;
    std::cout << "---------------------------------------"                  << std::endl;
    std::cout << "     Convexifications: " << data.convexifications         << std::endl;
    std::cout << "     Concavifications: " << data.concavifications         << std::endl;
    std::cout << "---------------------------------------"                  << std::endl;
    std::cout << "           Converged : " << data.converged                << std::endl;
    std::cout << "             Runtime : " << data.runtime << "s"           << std::endl;
    std::cout << "             Timeout : " << data.timeout                  << std::endl;
    std::cout << "---------------------------------------"                  << std::endl;
    std::cout << "        Flips (exact): " << data.flips_exact              << std::endl;
    std::cout << "       Flips (double): " << data.flips_double             << std::endl;
    std::cout << "Snap Roundings Failed: " << data.snap_roundings_failed    << std::endl;
    std::cout << ":::::::::::::::::::::::::::::::::::::::"                  << std::endl;

    if(filename!=NULL)
    {
        FILE *f = fopen(filename,"a");
        if(!f) return;
        fprintf(f,"%s ",data.m0.mesh_data().filename.c_str());
        fprintf(f,"%d ",domain);
        fprintf(f,"%d ",data.refinement_enabled);
        fprintf(f,"%d ",data.abort_if_too_slow);
        fprintf(f,"%f ",data.max_time_per_step);
        fprintf(f,"%d ",data.tris_in);
        fprintf(f,"%d ",data.tris_out);
        fprintf(f,"%f ",data.mesh_growth);
        fprintf(f,"%d ",data.moves_tot);
        fprintf(f,"%d ",data.moves_split);
        fprintf(f,"%d ",data.moves_flip);
        fprintf(f,"%d ",data.convexifications);
        fprintf(f,"%d ",data.concavifications);
        fprintf(f,"%d ",data.converged);
        fprintf(f,"%d ",data.timeout);
        fprintf(f,"%f ",data.runtime);
        fprintf(f,"%d ",data.flips_exact);
        fprintf(f,"%d ",data.flips_double);
        fprintf(f,"%d ",data.snap_roundings_failed);
        fprintf(f,"\n");
        fclose(f);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    std::string s = (argc>1) ? std::string(argv[1]) : std::string(DATA_PATH) + "/hele_shaw.obj";
    data.m0 = DrawableTrimesh<>(s.c_str());

    if(argc>=3) // batch mode: second argument is path to a text log file
    {
        // argv[1] => inupt mesh (must be a topological disk)
        // argv[2] => logfile
        // argv[3] => target domain (with the convention below)
        if(argc>3)
        {
            switch(atoi(argv[3]))
            {
                case 0: data.target_domain = CIRCLE; break;
                case 1: data.target_domain = SQUARE; break;
                case 2: data.target_domain = STAR;   break;
            }
        }
        data.p.push("AFM");
        AFM(data);
        data.runtime = data.p.pop();
        stats(argv[2]);
        return (data.converged?EXIT_SUCCESS:EXIT_FAILURE);
    }
    else
    {
        std::cout << "\nPress SPACE to make a single step" << std::endl;
        std::cout << "SHIFT+SPACE to run until convergence." << std::endl;
        std::cout << "Step size can be controlled from the control bar (press TAB to show it).\n" << std::endl;

        AFM_init(data);
        data.m0.updateGL();
        data.m1.updateGL();

        GLcanvas gui0,gui1;
        gui0.push(&data.m0);
        gui1.push(&data.m1);
        gui0.push(new SurfaceMeshControls<DrawableTrimesh<>>(&data.m0,&gui0,"m0"));
        gui0.push(new SurfaceMeshControls<DrawableTrimesh<>>(&data.m1,&gui0,"m1"));

        gui0.callback_key_pressed = [&](int key, int modifiers) -> bool
        {
            if(key==GLFW_KEY_SPACE)
            {
                data.step_by_step = !(modifiers==GLFW_MOD_SHIFT);
                data.p.push("AFM");
                AFM(data);
                data.runtime = data.p.pop();
                data.m0.updateGL();
                data.m1.updateGL();
                stats();
            }
            else if(key==GLFW_KEY_I) stats();
            else return false;
            return true;
        };

        int domain = 0;
        gui0.callback_app_controls = [&]()
        {
            if(ImGui::Combo("Domain", &domain, "CIRCLE\0SQUARE\0STAR\0"))
            {
                switch(domain)
                {
                    case 0 : data.target_domain = CIRCLE; break;
                    case 1 : data.target_domain = SQUARE; break;
                    case 2 : data.target_domain = STAR;   break;
                }
                AFM_init(data);
                data.m0.updateGL();
                data.m1.updateGL();
            }

            ImGui::SliderInt("Steps", &data.step_size, 1, 1000);
        };

        return gui0.launch({&gui1});
    }
}
