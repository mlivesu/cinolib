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
#ifndef CINO_RBF_HERMITE_H
#define CINO_RBF_HERMITE_H

#include <cinolib/geometry/vec3.h>
#include <cinolib/scalar_field.h>
#include <Eigen/Dense>

namespace cinolib
{

/* Implementation of Hermite RBF interpolation. Given a set of oriented points (p_i,n_i), i=0,1,..,n
 * find the implicit function that interpolates them.
 *
 * This code is almost 1:1 copied from the great tutorial of Rodolphe Vaillant, available at
 * http://rodolphe-vaillant.fr/?e=12
 *
 * Reference academic resources for Hermite RBF interpolation are:
 *
 *     Hermite Radial Basis Functions Implicits
 *     I. Macedo, J.P. Gois, L. Velho
 *     Computer Graphics Forum (2011)
 *
 *     and
 *
 *     A Closed-Form Formulation of HRBF-Based Surface Reconstruction
 *     S. Liu, C.C.L. Wang, G. Brunnett, J. Wang
 *     Computer-Aided Design (2016)
*/

template<class RBF>
class Hermite_RBF
{
    public:

        Hermite_RBF(){}
        Hermite_RBF(const std::vector<vec3d> & points,
                    const std::vector<vec3d> & normals);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        ScalarField eval     (const std::vector<vec3d> & plist) const; // evaluate RBF at points plist
        double      eval     (const vec3d & p) const;                  // evaluate RBF at point p
        vec3d       eval_grad(const vec3d & p) const;                  // evaluate nabla RBF at point p

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Eigen::VectorXd  alpha;  // vector of scalar values alpha
        Eigen::Matrix3Xd beta;   // each column represents beta_i: VectorX bi = beta.col(i);
        Eigen::Matrix3Xd center; // each column represents p_i:    VectorX pi = centers.col(i);
};

}

#ifndef  CINO_STATIC_LIB
#include "RBF_Hermite.cpp"
#endif

#endif // CINO_RBF_HERMITE_H
