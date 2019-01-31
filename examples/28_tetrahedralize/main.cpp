/* This is a simple tetrahedralization tool that reads a surface mesh and a set of
 * flags to pass along to tetgen
 *
 * Enjoy!
*/

#include <cinolib/meshes/meshes.h>
#include <cinolib/tetgen_wrap.h>
#include <cinolib/string_utilities.h>
#include <cinolib/stl_container_utilities.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    if(argc<2)
    {
        std::cout << "\n\nusage:\n\ttetrahedralize input [tetgen_flags]\n\n" << std::endl;;
        return -1;
    }

    Polygonmesh<> m(argv[1]);
    std::string flags = (argc<2) ? "" : argv[2];
    std::vector<vec3d> v_out;
    std::vector<uint>  e_in, p_out;
    tetgen_wrap(m.vector_verts(), m.vector_polys(), e_in, flags.c_str(), v_out, p_out);
    write_MESH((get_file_name(std::string(argv[1]), false) + ".mesh").c_str(), v_out, polys_from_serialized_vids(p_out,4));

    return 0;
}
