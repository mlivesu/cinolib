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
#ifndef CINO_RBF_H
#define CINO_RBF_H

#include <vector>
#include <cinolib/unstable/RBF_library.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

/* References:
 *
 * Modelling with Implicit Surfaces that Interpolate
 * G Turk, JF O'brien
 * SIGGRAPH 2002
 *
 * Reconstruction and Representation of 3D Objects with Radial Basis Functions
 * JC Carr, RK Beatson, JB Cherrie, TJ Mitchell, WR Fright, BC McCallum, TR Evans
 * SIGGRAPH 2001
 */

template<class Point>
class RBF_interpolation
{
    public:

        explicit RBF_interpolation(const AbstractRBF<vec3d>  * phi,
                                   const std::vector<Point>  & centers,
                                   const std::vector<double> & samples);

        double eval(const Point & p) const;

        bool function_interpolates_at_centers(const double epsilon) const;

    protected:

        void compute_w_and_P();

        const AbstractRBF<Point> *phi;
        const std::vector<Point>  centers;
        const std::vector<double> samples;

        std::vector<double> w;
        double              P[4];
};

}

#ifndef  CINO_STATIC_LIB
#include "RBF.cpp"
#endif

#endif // CINO_RBF_H
