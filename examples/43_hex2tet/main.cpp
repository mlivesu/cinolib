#include <cinolib/meshes/meshes.h>
#include <cinolib/tetrahedralization.h>
#include <cinolib/string_utilities.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char *argv[])
{
    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/ellipsoid.mesh";
    Hexmesh<> hm(s.c_str());
    Tetmesh<> tm;
    hex_to_tets(hm,tm);
    std::string basename = get_file_path(s,true);
    tm.save((basename+"_tet.mesh").c_str());
    return 0;
}
