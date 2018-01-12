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

#include <valarray>
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
template <class Point> class Gaussian_RBF : public AbstractRBF<Point>
{
    double sigma;

    public:

        explicit Gaussian_RBF(const double sigma) : sigma(sigma) {}

        double eval(const Point & center, const Point & sample) const
        {
            double d = center.dist(sample);
            return exp(-sigma*d*d);
        }
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Point> class Cubic_Spline_RBF : public AbstractRBF<Point>
{
    public:

        explicit Cubic_Spline_RBF() {}

        double eval(const Point & center, const Point & sample) const
        {
            double d = center.dist(sample);
            return d*d*d;
        }
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Point> class Thin_Plate_Spline_RBF : public AbstractRBF<Point>
{
    public:

        explicit Thin_Plate_Spline_RBF() {}

        double eval(const Point & center, const Point & sample) const
        {
            double d = center.dist(sample);
            return d*d*log(d);
        }
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Point> class Polyharmonic_Spline_RBF : public AbstractRBF<Point>
{
    uint k;

    public:

        explicit Polyharmonic_Spline_RBF(const uint k) : k(k) {}

        double eval(const Point & center, const Point & sample) const
        {
            double d = center.dist(sample);
            double f = std::pow(d,k);
            if (k%2==1) f *= log(d);
            return f;
        }
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class Point> class Multi_Quadratic_RBF : public AbstractRBF<Point>
{
    double eps;

    public:

        explicit Multi_Quadratic_RBF(const double eps) : eps(eps) {}

        double eval(const Point & center, const Point & sample) const
        {
            double d = center.dist(sample);
            return sqrt(1+(eps*d)*(eps*d));
        }
};

}

#ifndef  CINO_STATIC_LIB
#include "RBF_library.cpp"
#endif

#endif // CINO_RBF_LIBRARY_H
