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
