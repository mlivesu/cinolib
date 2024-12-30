/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2024: Marco Livesu                                              *
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
#ifndef CINO_STRIPE_EMBEDDING_H
#define CINO_STRIPE_EMBEDDING_H

#ifdef CINOLIB_USES_CGAL_GMP_MPFR

#include <cinolib/meshes/drawable_trimesh.h>
#include <mpreal.h>
#include <cinolib/rationals.h>

namespace cinolib
{

/* Reference implementation of the article
 *
 * Stripe Embedding: Efficient Maps with Exact Numeric Computation
 * Marco Livesu
 * ACM Transactions on Graphics (SIGGRAPH Asia 2024)
 *
 * Example of usage:
 *
 *     SE_data data;
 *     data.m = input_trimesh;
 *     stripe_embedding(data);
 *
 * For interactive use, call init(data) first and then call stripe_embedding(data).
 * See the dedicated example (#48) in cinolib/examples.
*/

struct SE_data
{
    DrawableTrimesh<> m; // input mesh (may be refined during map generation)

    // If provided, SE will use the input boundary conditions.
    // Otherwise, it will generate a map to the polygon indicated in target_domain
    std::map<uint,vec3d> bc;
    int target_domain = CIRCLE;
    std::vector<uint> boundary; // ordered boundary vertices of m

    // higher precision numerical models available
    bool use_rationals = false;
    bool use_MPFR      = false;

    mpfr_prec_t MPFR_precision = 512; // # of bits for the mantissa when MPFR is used

    // embeddings with the various numerical models
    std::vector<double>       coords_d;
    std::vector<CGAL_Q>       coords_q;
    std::vector<mpfr::mpreal> coords_m;

    // flipped elements' count, for all supported numeric types
    uint flips_d = 0;
    uint flips_q = 0;
    uint flips_m = 0;

    bool initialized  = false;
    bool stop         = false;
    bool step_by_step = false;
    int  step_size    = 1;

    std::queue<uint> q; // stripe espansion queue

    // this is only for visuals. The following vectors will be filled only if store_stripes is true
    std::vector<uint> stripes_offset; // stripe starting index
    std::vector<uint> stripes;        // pivot + serialized chain of vertices opposite to the pivot
    bool store_stripes = false;

    uint embedded_verts = 0;
    std::vector<bool> embedded;
    std::vector<int>  edge_chain_id;
    uint fresh_id = 1;

    // stats
    float runtime     = 0;
    bool  converged   = false;
    uint  edge_splits = 0;
    uint  iters       = 0;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void stripe_embedding(SE_data & data);

}

#ifndef  CINO_STATIC_LIB
#include "stripe_embedding.cpp"
#endif

#endif // CINOLIB_USES_CGAL_GMP_MPFR
#endif // CINO_STRIPE_EMBEDDING_H
