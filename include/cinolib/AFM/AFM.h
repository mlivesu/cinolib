/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2023: Marco Livesu                                              *
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
#ifndef CINO_AFM_H
#define CINO_AFM_H

#ifdef CINOLIB_USES_CGAL_GMP_MPFR

#include <cinolib/meshes/drawable_trimesh.h>
#include <cinolib/rationals.h>
#include <cinolib/profiler.h>

namespace cinolib
{

/* Reference implementation of the article
 *
 * Advancing Front Surface Mapping
 * Marco Livesu
 * Eurographics 2024
 *
 * Example of usage:
 *
 *     AFM_data data;
 *     data.m0 = input_trimesh;
 *     data.target_domain = CIRCLE | SQUARE | STAR
 *     AFM(data);
 *
 * For interactive use, call AFM_init(data) first and then call AFM(data), using data.stop to force the execution
 * of a finite number of steps. See the dedicated example (#47) in cinolib/examples.
*/

struct AFM_data
{
    DrawableTrimesh<>   m0;                     // input mesh. May be refined during map generation
    DrawableTrimesh<>   m1;                     // output mesh of the target domain, same connectivity as m0
    std::deque<uint>    front;                  // serialized front edges
    int                 target_domain = CIRCLE; // CIRCLE, SQUARE, STAR
    uint                origin;                 // id of the vertex selected as the origin of the front
    bool                initialized = false;    // true if m1 has already been initialized
    std::vector<CGAL_Q> exact_coords;           // rational coordinates for exact computation

    // profiling / debugging / step-by-step execution
    Profiler p;
    bool     enable_snap_rounding = true;  // snap to float anytime this is possible (i.e., no flips generated)
    bool     enable_sanity_checks = false; // toggle additional assertions for careful debug
    bool     step_by_step         = false; // toggle step-by-step execution
    int      step_size            = 1;     // moves for each step
    bool     stop                 = false; // if set to true, stops after current iteration (for debug)
    bool     refinement_enabled   = true;  // permit input mesh refinement to unlock deadlocks with convexification and concavification
    bool     abort_if_too_slow    = true;  // stop execution if a moves takes more than max_time_per_step
    double   max_time_per_step    = 2;     // seconds

    // statistics / colors
    uint  tris_in;
    uint  tris_out;
    float mesh_growth = 0;
    uint  moves_tot = 0;
    uint  moves_split = 0;
    uint  moves_flip = 0;
    uint  convexifications = 0;
    uint  concavifications = 0;
    bool  converged = false;
    bool  timeout = false;
    float runtime = 0;
    uint  flips_exact  = 0;
    uint  flips_double = 0;
    uint  snap_roundings_failed = 0;
    Color conquered_color = Color(193.f/255.f,238.f/255.f,1.f);
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void AFM(AFM_data & data);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void AFM_init(AFM_data & data);

}

#ifndef  CINO_STATIC_LIB
#include "AFM.cpp"
#endif

#endif // CINOLIB_USES_CGAL_GMP_MPFR
#endif // CINO_AFM_H
