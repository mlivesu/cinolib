#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>
#include <cinolib/drawable_segment_soup.h>
#include <cinolib/stripe_embedding/stripe_embedding.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void stats(SE_data & data, const char *filename = nullptr)
{
    float perc_d = 100.f*data.flips_d/data.m.num_polys();
    float perc_q = 100.f*data.flips_q/data.m.num_polys();
    float perc_m = 100.f*data.flips_m/data.m.num_polys();
    // round to the second digit
    perc_d = floorf(perc_d*100)/100;
    perc_q = floorf(perc_q*100)/100;
    perc_m = floorf(perc_m*100)/100;

    std::cout << ":::::::::::::::: STATS ::::::::::::::::"         << std::endl;
    std::cout << "                Verts: " << data.m.num_verts()   << std::endl;
    std::cout << "                 Tris: " << data.m.num_polys()   << std::endl;
    std::cout << "          Edge Splits: " << data.edge_splits     << std::endl;
    std::cout << "---------------------------------------"         << std::endl;
    std::cout << "           Iterations: " << data.iters           << std::endl;
    std::cout << "             Runtime : " << data.runtime << "s"  << std::endl;
    std::cout << "           Converged : " << data.converged       << std::endl;
    std::cout << "---------------------------------------"         << std::endl;
    std::cout << "            rationals: " << data.use_rationals   << std::endl;
    std::cout << "                 MPFR: " << data.use_MPFR        << std::endl;
    std::cout << "       MPFR precision: " << data.MPFR_precision  << std::endl;
    std::cout << "---------------------------------------"         << std::endl;
    std::cout << "     Flips   (double): " << data.flips_d << " (" << perc_d << "%)" << std::endl;
    std::cout << "     Flips (rational): " << data.flips_q << " (" << perc_q << "%)" << std::endl;
    std::cout << "     Flips     (MPFR): " << data.flips_m << " (" << perc_m << "%)" << std::endl;
    std::cout << ":::::::::::::::::::::::::::::::::::::::"              << std::endl;

    if(filename!=NULL)
    {
        FILE *f = fopen(filename,"a");
        if(!f) return;
        fprintf(f,"%s ",data.m.mesh_data().filename.c_str());
        fprintf(f,"%d ",data.m.num_verts());
        fprintf(f,"%d ",data.edge_splits);
        fprintf(f,"%f ",data.runtime);
        fprintf(f,"%d ",data.converged);
        fprintf(f,"%d ",data.flips_d);
        fprintf(f,"%d ",data.flips_q);
        fprintf(f,"%d ",data.flips_m);
        fprintf(f,"\n");
        fclose(f);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    SE_data data;
    std::string s = (argc>1) ? std::string(argv[1]) : std::string(DATA_PATH) + "/hele_shaw.obj";
    data.m = DrawableTrimesh<>(s.c_str());

    if(argc>=3) // batch mode
    {
        // argv[1] => inupt mesh (must be a topological disk)
        // argv[2] => path to a logfile
        // argv[3] => 1 : enable rationals
        //         => 2 : enable MPFR
        // argv[4] => MPFR precision
        if(argc>3)
        {
            switch(atoi(argv[3]))
            {
                case 1 : data.use_rationals = true; break;
                case 2 : data.use_MPFR      = true; break;
            }
            if(argc>4) data.MPFR_precision = mpfr_prec_t(atoi(argv[4]));
        }
        stripe_embedding(data);
        stats(data,argv[2]);
        return (data.converged?EXIT_SUCCESS:EXIT_FAILURE);
    }
    else
    {
        std::cout << "\nPress SPACE to make a single step" << std::endl;
        std::cout << "SHIFT+SPACE to run until convergence." << std::endl;
        std::cout << "Step size can be controlled from the control bar (press TAB to show it).\n" << std::endl;

        data.store_stripes = true; // for visualization
        data.step_by_step  = true;

        DrawableTrimesh<>res = data.m;
        res.poly_set_flag(HIDDEN,true);
        res.updateGL();

        GLcanvas gui0,gui1;
        gui0.show_side_bar = true;
        gui0.push(&data.m);
        gui1.push(&res);
        gui0.push(new SurfaceMeshControls<DrawableTrimesh<>>(&data.m,&gui0,"m0"));
        gui0.push(new SurfaceMeshControls<DrawableTrimesh<>>(&res,&gui0,"m1"));

        DrawableSegmentSoup chains0, chains1;
        chains0.use_gl_lines = true;
        chains0.default_color = Color::BLACK();
        chains0.thickness = 4;
        chains1 = chains0;
        gui0.push(&chains0,false);
        gui1.push(&chains1,false);

        int prec = data.MPFR_precision;
        gui0.callback_app_controls = [&]()
        {
            if(ImGui::SliderInt("Step size",&data.step_size,1,1000))  {}
            if(ImGui::Checkbox ("Use Rationals",&data.use_rationals)) {}
            if(ImGui::Checkbox ("Use MPFR",&data.use_MPFR))           {}
            if(ImGui::SliderInt("MPFR precision",&prec,16,1024)) { data.MPFR_precision = mpfr_prec_t(prec); }
        };

        gui0.callback_key_pressed = [&](int key, int modifiers) -> bool
        {
            if(key==GLFW_KEY_SPACE)
            {
                data.step_by_step = !(modifiers==GLFW_MOD_SHIFT);
                stripe_embedding(data);
                if(data.converged) stats(data);

                // copy coords to res mesh
                for(uint vid=0; vid<data.m.num_verts(); ++vid)
                {
                    res.vert(vid).x() = data.coords_d.at(2*vid  );
                    res.vert(vid).y() = data.coords_d.at(2*vid+1);
                    res.vert(vid).z() = 0.0;
                }
                // udate visual chains
                chains0.clear();
                chains1.clear();
                uint n_stripes = data.stripes_offset.size();
                for(uint i=0; i<n_stripes; ++i)
                {
                    uint  beg = data.stripes_offset.at(i);
                    uint  end = (i<n_stripes-1) ? data.stripes_offset.at(i+1) : data.stripes.size();
                    uint  piv = data.stripes.at(beg);
                    Color c   = Color::scatter(n_stripes,i);
                    for(uint j=beg+2; j<end; ++j)
                    {
                        uint c0 = data.stripes.at(j-1);
                        uint c1 = data.stripes.at(j  );
                        int  pid = data.m.poly_id({piv,c0,c1});
                        assert(pid>=0);
                        data.m.poly_data(pid).color = c;
                        res.poly_data(pid).color = c;
                        chains0.push_seg(data.m.vert(c0),data.m.vert(c1));
                        chains1.push_seg(res.vert(c0),res.vert(c1));
                    }
                }
                // show embedded triangles
                for(uint pid=0; pid<res.num_polys(); ++pid)
                {
                    if(data.embedded.at(res.poly_vert_id(pid,0)) &&
                       data.embedded.at(res.poly_vert_id(pid,1)) &&
                       data.embedded.at(res.poly_vert_id(pid,2)))
                    {
                        res.poly_data(pid).flags[HIDDEN] = false;
                    }
                }
                data.m.updateGL();
                res.updateGL();
                res.update_bbox();
                gui1.refit_scene();
            }
            else if(key==GLFW_KEY_I) stats(data);
            else return false;
            return true;
        };
        return gui0.launch({&gui1});
    }
}
