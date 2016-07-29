/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#include <cinolib/smoothing.h>

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

template<class Mesh>
CINO_INLINE
void smooth_taubin(Mesh                & m,
                   const std::set<int>   do_not_smooth =  std::set<int>(),
                   const int             mode          =  COTANGENT,
                   const int             n_iters       =  10,
                   const double          lambda        =  0.9,
                   const double          mu            = -0.9)
{
    assert(lambda > 0.0);
    assert(mu     < 0.0);

    std::vector<vec3d>  delta(m.num_vertices(), vec3d(0,0,0));
    std::vector<double> sum(m.num_vertices(), 0.0);

    for(int iter=0; iter<n_iters; ++iter)
    {
        // shrink
        //
        for(int vid=0; vid<m.num_vertices(); ++vid)
        {
            if (CONTAINS(do_not_smooth, vid)) continue;

            std::vector<int>    nbrs;
            std::vector<double> wgts;
            if (mode & UNIFORM)   uniform_weights<Mesh>  (m, vid, nbrs, wgts); else
            if (mode & COTANGENT) cotangent_weights<Mesh>(m, vid, nbrs, wgts); else
            assert(false);
            for(size_t i=0; i<nbrs.size(); ++i)
            {
                delta[vid] += (m.vertex(vid) - m.vertex(nbrs[i])) * wgts[i];
                sum[vid]   += wgts[i];
            }
        }
        for(int vid=0; vid<m.num_vertices(); ++vid)
        {
            m.set_vertex(vid, m.vertex(vid) + delta[vid] * lambda / sum[vid]);
            delta[vid] = vec3d(0,0,0);
            sum[vid]   = 0.0;
        }

        // inflate
        //
        for(int vid=0; vid<m.num_vertices(); ++vid)
        {
            if (CONTAINS(do_not_smooth, vid)) continue;

            std::vector<int>    nbrs;
            std::vector<double> wgts;
            if (mode & UNIFORM)   uniform_weights<Mesh>  (m, vid, nbrs, wgts); else
            if (mode & COTANGENT) cotangent_weights<Mesh>(m, vid, nbrs, wgts); else
            assert(false);
            for(size_t i=0; i<nbrs.size(); ++i)
            {
                delta[vid] += (m.vertex(vid) - m.vertex(nbrs[i])) * wgts[i];
                sum[vid]   += wgts[i];
            }
        }
        for(int vid=0; vid<m.num_vertices(); ++vid)
        {
            m.set_vertex(vid, m.vertex(vid) + delta[vid] * mu / sum[vid]);
            delta[vid] = vec3d(0,0,0);
            sum[vid]   = 0.0;
        }
   }    
}

}
