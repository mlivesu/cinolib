#ifndef CINO_HEX_TRANSITION_H
#define CINO_HEX_TRANSITION_H

#include <cinolib/hex_transition_schemes.h>
#include <cinolib/meshes/meshes.h>

namespace cinolib{

enum class HexTransition
{
    // schemes for strongly balanced grids
    FLAT,
    FLAT_CONVEX,
    CONVEX_1,
    CONVEX_2,
    CONVEX_3,
    EDGE,
    VERT_CENTER,
    VERT_SIDE,

    // additional schemes for weakly balanced grids
    EDGE_WB,
    VERT_CENTER_WB_1,
    VERT_CENTER_WB_2,
    VERT_CENTER_WB_3,
    VERT_SIDE_WB
};

enum class SchemeType
{
    FLAT_S,
    CONC_S,
    CORN_S,
    CONV_S
};

struct SchemeInfo{
    HexTransition        type;
    SchemeType           scheme_type;
    double               scale;
    std::vector<vec3d>   t_verts;
    std::map<uint, bool> cuts;
    std::vector<int>     orientations;
};

inline bool eps_eq(const cinolib::vec3d &a, const cinolib::vec3d &b, double eps = 1e-6){
    return (a.dist(b) <= eps);
}


/*This function sets a hex transition scheme to the right position, scale and orientation. The parameters are:
 * -verts, faces, polys, winding  => geometry and connectivity info of the scheme specified in info.
 * -info                          => struct containing all the information needed to place a scheme correctly.
 * -application_poly_centroid     => the centroid of the hexahedron that will be replaced with the transition scheme
 */
CINO_INLINE
void orient_hex_transition(std::vector<vec3d>        &verts,
                                 std::vector<std::vector<uint>> &faces,
                                 std::vector<std::vector<uint>> &polys,
                                 std::vector<std::vector<bool>> &winding,
                                 const SchemeInfo &info,
                                 const vec3d application_poly_centroid);
}

#ifndef  CINO_STATIC_LIB
#include "hex_transition.cpp"
#endif

#endif // CINO_HEX_TRANSITION_H
