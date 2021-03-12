#ifndef CINO_HEX_TRANSITION_SCHEMES_INSTALL_H
#define CINO_HEX_TRANSITION_SCHEMES_INSTALL_H


#include <cinolib/hex_transition.h>
namespace cinolib {

/* This function installs the hex transition schemes defined in hex_transition_schemes.h
 * to turn an adaptive grid with hanging nodes into a conformin all hexahedral mesh. The
 * function takes 3 parameters:
 * -m                => The input grid (must be a polyhedral mesh since the schemes are composed of generic polyhedra)
 * -transition_verts => vector of booleans with true values in correspondence of transition vertices. A transition
 * 			vertex in the grid is a vertex with 8 different hexahedra, four with size n and four with size n-1.
 * 			The transition vertices must be selected carefully in order achieve a correct installation.
 * -out 	     => The output mesh with schemes installed.
 */

template<class M, class V, class E, class F, class P>
CINO_INLINE
void hex_transition_schemes_install(const Polyhedralmesh<M,V,E,F,P> &m, const std::vector<bool> &transition_verts, Polyhedralmesh<M,V,E,F,P> &out);


//Custom comparator operator for maps of vec3d
struct vert_compare{
    bool operator()(const vec3d& a, const vec3d& b) const {

       double eps = 1e-6;
       if(a.x()-b.x() < 0.0 && abs(a.x()-b.x()) > eps){
           return true;
       }
       else if(abs(a.x()-b.x()) < eps){
           if(a.y()-b.y() < 0.0 && abs(a.y()-b.y()) > eps){
               return true;
           }
           else if(abs(a.y()-b.y()) < eps){
               if(a.z()-b.z() < 0.0 && abs(a.z()-b.z()) > eps){
                   return true;
               }
           }
       }

       return false;
    }
};

}

#ifndef  CINO_STATIC_LIB
#include "hex_transition_schemes_install.cpp"
#endif


#endif // CINO_HEX_TRANSITION_SCHEMES_INSTALL_H
