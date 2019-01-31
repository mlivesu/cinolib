/* This program maps substitutes the skin of a volumetric mesh with an input surface mesh.
 * The correspondences between vertices in the surface and volumetric meshes are provided
 * with a text file.
 *
 * Enjoy!
*/

#include <cinolib/meshes/meshes.h>
#include <cinolib/string_utilities.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    if(argc!=5)
    {
        std::cout << "\n\nusage:\n\timport_surface -tet|hex|poly volume_mesh surface_mesh vertex_map\n\n" << std::endl;;
        return -1;
    }

    AbstractPolyhedralMesh<> *vol = NULL;
    Polygonmesh<> srf(argv[3]);

         if(strcmp(argv[1],"-tet" )==0) vol = new Tetmesh<>(argv[2]);
    else if(strcmp(argv[1],"-hex" )==0) vol = new Hexmesh<>(argv[2]);
    else if(strcmp(argv[1],"-poly")==0) vol = new Polyhedralmesh<>(argv[2]);
    else
    {
        std::cout << "Unknown extension" << std::endl;
        return -1;
    }

    std::ifstream f(argv[4]);
    if(f.is_open())
    {
        std::string line;
        while(getline(f, line, '\n'))
        {
            std::stringstream ss(line);
            uint srf_vid, vol_vid;
            assert(ss >> srf_vid >> vol_vid);
            vol->vert(vol_vid) = srf.vert(srf_vid);
            if(!vol->vert_is_on_srf(vol_vid))
            {
                std::cerr << "WARNINNG: " << vol_vid << " is not a surface vertex!" << std::endl;
            }
        }
    }

    vol->save((get_file_name(argv[2],false) + "_new_srf." + get_file_extension(argv[2])).c_str());

    return 0;
}
