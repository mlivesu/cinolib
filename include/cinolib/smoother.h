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
#ifndef CINO_SMOOTHER_H
#define CINO_SMOOTHER_H

#include <cinolib/meshes/meshes.h>

namespace cinolib
{

/* Smooths a given polygonal mesh minimizing the same fairing energy
 * described in:
 *
 *   Practical Hex-Mesh Optimization via Edge-Cone Rectification
 *   Marco Livesu, Alla Sheffer, Nicholas Vining, Marco Tarini
 *   ACM Transactions on Graphics (SIGGRAPH 2015)
 *
 * Mesh vertices are grouped in 3 categories:
 *
 *  - R: regular vertices, to be smooth in the tangent space
 *  - F: feature vertices, to be smooth along the sharp crease they belong to
 *  - C: corner vertices, at the intersection of multiple features, to be held in place
 *
 * NOTE: feature lines are detected as chains of consecutive edges marked in the mesh.
 * While the classification is done internally, proper edge marking/unmarking must be
 * done prior calling the smother.
 *
 * The energy being minimized is the following :
 *
 * E_smooth = w_curr_pos  * E_curr_pos  +
 *            w_laplacian * E_laplacian +
 *            w_regular   * E_regular   +
 *            w_feature   * E_feature   +
 *            w_corner    * E_corner
 *
 * E_curr_pos = \sum_{\forall i} (v_i - v_i*)^2,
 * where v_i* is the current position of v_i
 *
 * E_laplacian = \sum_{\forall i} \sum_{\forall j \in N(i)} (v_i - v_j)^2
 *
 * E_regular = \sum_{\forall i \in R} (n*v_i + d)^2,
 * where <n,d> is the plane tangent to the mesh at v_i
 *
 * E_feature = \sum_{\forall i \in F} (v_i - (v_i + t*d))^2 + t^2,
 * where <t,d> is the line L::= v_i + t*d tangent to the crase at v_i, parameterized by the extra varaible t
 *
 * E_corner = \sum_{\forall i \in C} (v_i - v_i*)^2,
 * where v_i* is the current position of v_i
*/

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

enum { REGULAR, CORNER, FEATURE };

typedef struct
{
    uint   n_iters             = 10;      // # of smoothing iterations
    double w_curr_pos          = 10.0;    // attraction to current position
    double w_regular           = 10.0;    // attraction to tangent space for regular vertices
    double w_feature           = 100.0;   // attraction to tangent curve for feature vertices
    double w_corner            = 100.0;   // attraction to current pos for features corner
    double w_laplace           = 0.001;   // weight of laplacian energy terms
    int    laplacian_mode      = UNIFORM; // laplacian mode (UNIFORM or COTANGENT)
    bool   reproject_on_target = true;    // reproject to target surface after each smoothing iteration
    //bool   with_ray_casting    = false;   // reproject via aray casting if true, via closest point if false
}
SmootherOptions;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M1, class V1, class E1, class P1,
         class M2, class V2, class E2, class P2>
CINO_INLINE
void mesh_smoother(      AbstractPolygonMesh<M1,V1,E1,P1> & m,
                   const AbstractPolygonMesh<M2,V2,E2,P2> & target,
                   const SmootherOptions                  & opt = SmootherOptions());
}

#ifndef  CINO_STATIC_LIB
#include "smoother.cpp"
#endif

#endif // CINO_SMOOTHER_H
