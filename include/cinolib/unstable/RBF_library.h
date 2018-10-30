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
