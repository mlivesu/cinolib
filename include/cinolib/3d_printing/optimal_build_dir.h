/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2022: Marco Livesu                                              *
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
#ifndef CINO_OPTIMAL_BUILD_DIR_H
#define CINO_OPTIMAL_BUILD_DIR_H

#include <cinolib/meshes/drawable_trimesh.h>

namespace cinolib
{

/* This metod returns the optimal build orientation for 3D printing according to
 * a linear combination of four typical metrics, that is:
 *
 *  - part height          => mimimizes part height along the build direction,
 *                            reducing the amount of slices hence decreasing printing time
 *
 *  - shadow area          => minimizes the footprint in the printing bed
 *                            (useful for packing multiple objects)
 *
 *  - support contact area => minimize the overhanging surface that necessitates external supports,
 *                            this counts also the area of upfacing triangles that have some overhang
 *                            above them and will be the "basis" for external supports
 *
 *  - supports volume      => minimizes the voume of external supports, thus reducing
 *                            the amount of material necessary to complete the print
 *
 * The algorithm is pretty straightforward: candidate directions are evently sampled
 * from the unit sphere. For each candidate build direction these four metrics are
 * evaluated. The output result is the direction that minimizes the energy.
 *
 * Users can choose how many directions should be tested, and what is the importance of
 * each metric in the global energy.
*/

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

struct OptimalBuildDirOptions
{
    uint  n_dirs             = 300;  // # of candidate build directions to test
    uint  buffer_size        = 128;  // size of the rasterization buffer (used for shadow area)
    float overhang_threshold = 30.0; // deg
    float w_height           = 0.25;
    float w_shadow_area      = 0.25;
    float w_support_contact  = 0.25;
    float w_support_volume   = 0.25;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
vec3d optimal_build_dir(const DrawableTrimesh<M,V,E,P> & m,
                        const OptimalBuildDirOptions   & opt);
}

#ifndef  CINO_STATIC_LIB
#include "optimal_build_dir.cpp"
#endif

#endif // CINO_OPTIMAL_BUILD_DIR_H
