/*********************************************************************************
*  Copyright(C) 2016: Marco Livesu                                               *
*  All rights reserved.                                                          *
*                                                                                *
*  This file is part of CinoLib                                                  *
*                                                                                *
*  CinoLib is dual-licensed:                                                     *
*                                                                                *
*   - For non-commercial use you can redistribute it and/or modify it under the  *
*     terms of the GNU General Public License as published by the Free Software  *
*     Foundation; either version 3 of the License, or (at your option) any later *
*     version.                                                                   *
*                                                                                *
*   - If you wish to use it as part of a commercial software, a proper agreement *
*     with the Author(s) must be reached, based on a proper licensing contract.  *
*                                                                                *
*  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE       *
*  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.     *
*                                                                                *
*  Author(s):                                                                    *
*                                                                                *
*     Marco Livesu (marco.livesu@gmail.com)                                      *
*     http://pers.ge.imati.cnr.it/livesu/                                        *
*                                                                                *
*     Italian National Research Council (CNR)                                    *
*     Institute for Applied Mathematics and Information Technologies (IMATI)     *
*     Via de Marini, 6                                                           *
*     16149 Genoa,                                                               *
*     Italy                                                                      *
**********************************************************************************/
#include <cinolib/unstable/smoothing.h>
#include <cinolib/laplacian.h>
#include <cinolib/stl_container_utilities.h>

namespace cinolib
{

/*
 * Implementation of Taubin Smoothing.
 *
 * Reference:
 * Curve and Surface Smoothing Without Shrinkage
 * Gabriel Taubin
 * Proceeedings of IEEE 5th International Conference on Computer Vision, 1995.
 *
*/

template<class M, class V, class E, class P>
CINO_INLINE
void smooth_taubin(AbstractMesh<M,V,E,P> & m,
                   const int               mode,
                   const std::set<uint>    do_not_smooth,
                   const uint              n_iters,
                   const double            lambda,
                   const double            mu)
{
    assert(lambda >  0 );
    assert(lambda <  1 );
    assert(lambda < -mu);

    for(uint iter=0; iter<n_iters; ++iter)
    {
        // shrink
        //
        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            if (CONTAINS(do_not_smooth, vid)) continue;

            std::vector<std::pair<uint,double>> wgts;
            m.vert_weights(vid, mode, wgts);

            // normalize weights...
            double sum = 0;
            for(auto  w : wgts) sum += w.second;
            for(auto &w : wgts) w.second /= sum;

            vec3d delta(0,0,0);
            for(auto w : wgts) delta += (m.vert(w.first) - m.vert(vid)) * w.first;
            m.vert(vid) = m.vert(vid) + delta * lambda;
        }

        // inflate
        //
        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            if (CONTAINS(do_not_smooth, vid)) continue;

            std::vector<std::pair<uint,double>> wgts;
            m.vert_weights(vid, mode, wgts);

            // normalize weights...
            double sum = 0;
            for(auto  w : wgts) sum += w.second;
            for(auto &w : wgts) w.second /= sum;

            vec3d delta(0,0,0);
            for(auto w : wgts) delta += (m.vert(w.first) - m.vert(vid)) * w.first;
            m.vert(vid) = m.vert(vid) + delta * mu;
        }
   }
}

}
