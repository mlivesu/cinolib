/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
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
