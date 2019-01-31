/* This program reads a volumetric mesh from file, and saves on the disk
 * a copy of its outer skin in the form of a polygon mesh. Using the additional
 * parameter -vmap it also exports a text file containing per vertex correspondences
 * between the volumetric and surface mesh
 *
 * Enjoy!
*/

#include <cinolib/meshes/meshes.h>
#include <cinolib/export_surface.h>
#include <cinolib/string_utilities.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    if(argc<3)
    {
        std::cout << "\n\nUsage:\n\texport_surface -tet|hex|poly filename [-vmap]\n\n" << std::endl;;
        return -1;
    }

    std::string filename(argv[2]);
    std::string base_name = get_file_name(filename,false);

    Polygonmesh<> srf;
    std::unordered_map<uint,uint> m2srf,srf2m;

    bool export_map = argc>3 && strcmp(argv[3],"-vmap")==0;

    if(strcmp(argv[1],"-tet")==0)
    {
        Tetmesh<> m(filename.c_str());
        export_surface(m, srf, m2srf, srf2m);
    }
    else if(strcmp(argv[1],"-hex")==0)
    {
        Hexmesh<> m(filename.c_str());
        export_surface(m, srf, m2srf, srf2m);
    }
    else if(strcmp(argv[1],"-poly")==0)
    {
        Polyhedralmesh<> m(filename.c_str());
        export_surface(m, srf, m2srf, srf2m);
    }
    else
    {
        std::cout << "Unknown extension" << std::endl;
        return -1;
    }

    srf.save((base_name + ".obj").c_str());

    if(export_map)
    {
        FILE *f = fopen((base_name + "_srf2vol_vmap.txt").c_str(), "w");
        if(f)
        {
            for(auto v2v : srf2m) fprintf(f,"%d %d\n", v2v.first, v2v.second);
            fclose(f);
        }
    }

    return 0;
}
