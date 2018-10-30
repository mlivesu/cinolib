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
#include "RBF.h"
#include <cinolib/linear_solvers.h>

namespace cinolib
{

template<class Point>
CINO_INLINE
RBF_interpolation<Point>::RBF_interpolation(const AbstractRBF<vec3d>  * phi,
                                            const std::vector<Point>  & centers,
                                            const std::vector<double> & samples)
    : phi(phi)
    , centers(centers)
    , samples(samples)
{
    assert(centers.size() == samples.size());
    compute_w_and_P();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Point>
CINO_INLINE
double RBF_interpolation<Point>::eval(const Point & p) const
{
    double f_val = 0.0;
    for(uint i=0; i<centers.size(); ++i)
    {
        f_val += w.at(i) * phi->eval(centers.at(i),p);
    }
    f_val += P[0];
    f_val += P[1] * p.x();
    f_val += P[2] * p.y();
    f_val += P[3] * p.z();
    return f_val;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Point>
CINO_INLINE
void RBF_interpolation<Point>::compute_w_and_P()
{
    uint RBF_weights      = centers.size();
    uint RBF_coefficients = 4; // P(c) = p0 + p1*cx + p2*cy + P3*cz
    uint size             = RBF_weights + RBF_coefficients;

    Eigen::SparseMatrix<double> A(size,size);
    Eigen::VectorXd b = Eigen::VectorXd::Zero(size);

    typedef Eigen::Triplet<double> Entry;
    std::vector<Entry> entries;

    uint row=0;
    for(; row<centers.size(); ++row)
    {
        b[row] = samples.at(row);

        vec3d ci  = centers.at(row);
        uint  col = 0;
        for(; col<centers.size(); ++col)
        {
            vec3d cj = centers.at(col);
            entries.push_back(Entry(row,col,phi->eval(ci,cj)));
        }

        entries.push_back(Entry(row,col,   1.0)); ++col;
        entries.push_back(Entry(row,col,ci.x())); ++col;
        entries.push_back(Entry(row,col,ci.y())); ++col;
        entries.push_back(Entry(row,col,ci.z()));
    }

           for(uint col=0; col<centers.size(); ++col) entries.push_back(Entry(row,col,1.0));
    ++row; for(uint col=0; col<centers.size(); ++col) entries.push_back(Entry(row,col,centers.at(col).x()));
    ++row; for(uint col=0; col<centers.size(); ++col) entries.push_back(Entry(row,col,centers.at(col).y()));
    ++row; for(uint col=0; col<centers.size(); ++col) entries.push_back(Entry(row,col,centers.at(col).z()));

    A.setFromTriplets(entries.begin(), entries.end());

    Eigen::VectorXd x;
    solve_square_system(A,b,x,BiCGSTAB);

    uint i=0;
    for(         ; i<RBF_weights;     ++i) w.push_back(x[i]);
    for(uint j=0; j<RBF_coefficients; ++j) P[j] = x[i++];

    //std::cout << A << std::endl; std::cout << std::endl;
    //std::cout << b << std::endl; std::cout << std::endl;
    //std::cout << x << std::endl; std::cout << std::endl;
    //std::cout << "W:" << std::endl;
    //for(auto wi : w) std::cout << wi << std::endl;
    //std::cout << "P:" << std::endl;
    //std::cout << P[0] << " " << P[1] << " " << P[2] << " " << P[3] << std::endl;
    //std::cout << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Point>
CINO_INLINE
bool RBF_interpolation<Point>::function_interpolates_at_centers(const double epsilon) const
{
    for(uint i=0; i<centers.size(); ++i)
    {
        double f   = eval(centers.at(i));
        double err = std::fabs(f - samples.at(i));
        std::cout << "f: " << f << "\t\tground truth:" << samples.at(i) << "\t\terr: " << err << std::endl;
        if (err > epsilon) return false;
    }
    return true;
}

}
