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
#include <cinolib/ssvd.h>
#include <cinolib/matrix.h>

namespace cinolib
{

CINO_INLINE
void ssvd(const Eigen::Matrix3d                 & m,
                Eigen::Matrix3d                 & u,
                Eigen::DiagonalMatrix<double,3> & s,
                Eigen::Matrix3d                 & v)
{
    Eigen::JacobiSVD<Eigen::Matrix3d> svd(m, Eigen::ComputeFullU | Eigen::ComputeFullV);

    s = Eigen::DiagonalMatrix<double,3>(svd.singularValues());
    u = svd.matrixU();
    v = svd.matrixV();

    if (v.determinant() < 0)
    {
        v.coeffRef(0,2) = -v.coeff(0,2);
        v.coeffRef(1,2) = -v.coeff(1,2);
        v.coeffRef(2,2) = -v.coeff(2,2);

        u.coeffRef(0,2) = - u.coeff(0,2);
        u.coeffRef(1,2) = - u.coeff(1,2);
        u.coeffRef(2,2) = - u.coeff(2,2);
    }

    assert(v.determinant() > 0);

    if (u.determinant() < 0)
    {
        u.coeffRef(0,2) = -u.coeff(0,2);
        u.coeffRef(1,2) = -u.coeff(1,2);
        u.coeffRef(2,2) = -u.coeff(2,2);

        s.diagonal()[2] = -s.diagonal()[2];
    }

    assert(u.determinant() > 0);

    //assert(m.determinant() * (u * s * v.transpose()).determinant() > 0);
    //cout << "m :\n" << m << std::endl;
    //cout << std::endl;
    //cout << "m':\n" << u * s * v.transpose() << std::endl;
    //cout << std::endl;
    //cout << "m' - m:\n" << m - (u * s * v.transpose()) << std::endl;
    //cout << std::endl;
    //cout << "det m: " << m.determinant() << std::endl;
    //cout << std::endl;
    //cout << "det m': " << (u * s * v.transpose()).determinant() << std::endl;
}


CINO_INLINE
void closest_pos_det_matrix(double T[3][3])
{
    Eigen::Matrix3d t;
    from_std_3x3_to_Eigen_3x3(T, t);

    if (t.determinant() > 0.0) return;

    Eigen::Matrix3d u, v;
    Eigen::DiagonalMatrix<double,3> s;

    ssvd(t, u, s, v);

    //cout << ":::::::::::::::::::::::::::::::::::::::::::" << std::endl;
    //cout << T[0][0] << "\t" << T[0][1] << "\t" << T[0][2] << std::endl;
    //cout << T[1][0] << "\t" << T[1][1] << "\t" << T[1][2] << std::endl;
    //cout << T[2][0] << "\t" << T[2][1] << "\t" << T[2][2] << std::endl;

    assert(s.diagonal()[0] > 0.0);
    assert(s.diagonal()[1] > 0.0);

    double avg_scale = (s.diagonal()[0] + s.diagonal()[1]) * 0.5;
    s.diagonal()[0] = avg_scale;
    s.diagonal()[1] = avg_scale;
    s.diagonal()[2] = avg_scale;

    t = u * s * v.transpose();

    assert(s.diagonal()[0] > 0.0);
    assert(s.diagonal()[1] > 0.0);
    assert(s.diagonal()[2] > 0.0);
    assert(t.determinant() > 0.0);

    from_eigen_3x3_to_std_3x3(t, T);
}

}
