#include <cinolib/meshes/meshes.h>

int main(int argc, char **argv)
{
    using namespace cinolib;

    if(argc<2)
    {
        std::cout << "\n\nusage:\n\tnormalize_and_center input output\n\n" << std::endl;;
        return -1;
    }
    Polygonmesh<> m(argv[1]);
    m.translate(-m.bbox().center());
    m.normalize_bbox();
    m.save(argv[2]);
    return 0;
}
