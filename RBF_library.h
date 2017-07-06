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
#ifndef CINO_RBF_LIBRARY_H
#define CINO_RBF_LIBRARY_H

#include <cinolib/cinolib.h>
#include <cmath>

/*
 * https://en.wikipedia.org/wiki/Radial_basis_function
*/

namespace cinolib
{

template <class Point> class AbstractRBF
{
    public: virtual double eval(const Point & center, const Point & sample) const = 0;
};


/* WARNING: notoriously unstable for small sigma values!! See:
 * Stable Evaluation of Gaussian Radial Basis Function Interpolants
 * SIAM Journal on Scientific Computing 34(2), 2012
 * GE Fasshauer, MJ McCourt
*/
template <class Point> class GaussianRBF : public AbstractRBF<Point>
{
    double sigma;

    public:

        GaussianRBF(const double sigma) : sigma(sigma) {}

        double eval(const Point & center, const Point & sample) const
        {
            double d = center.dist(sample);
            return exp(-sigma*d*d);
        }
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Point> class CubicSplineRBF : public AbstractRBF<Point>
{
    public:

        CubicSplineRBF() {}

        double eval(const Point & center, const Point & sample) const
        {
            double d = center.dist(sample);
            return d*d*d;
        }
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Point> class ThinPlateSplineRBF : public AbstractRBF<Point>
{
    public:

        ThinPlateSplineRBF() {}

        double eval(const Point & center, const Point & sample) const
        {
            double d = center.dist(sample);
            return d*d*log(d);
        }
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Point> class MultiQuadraticRBF : public AbstractRBF<Point>
{
    public:

        MultiQuadraticRBF() {}

        double eval(const Point & center, const Point & sample) const
        {
            double d = center.dist(sample);
            return sqrt(1+d*d);
        }
};

}

#ifndef  CINO_STATIC_LIB
#include "RBF_library.cpp"
#endif

#endif // CINO_RBF_LIBRARY_H
