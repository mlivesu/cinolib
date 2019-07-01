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
#include <cinolib/RBF_Hermite.h>

namespace cinolib
{

template<class RBF>
CINO_INLINE
Hermite_RBF<RBF>::Hermite_RBF(const std::vector<vec3d> & points,
                              const std::vector<vec3d> & normals)
{
    assert(points.size()==normals.size());

    uint np = points.size();
    alpha.resize(np);
    beta.resize(3, np);
    center.resize(3, np);

    uint size = 4*np;
    Eigen::MatrixXd A(size, size);
    Eigen::VectorXd f(size);
    Eigen::VectorXd x(size);

    // copy the node centers
    for(uint i=0; i<np; ++i)
    {
        center.col(i) = Eigen::Vector3d(points.at(i).x(), points.at(i).y(), points.at(i).z());
    }

    for(uint i=0; i<np; ++i)
    {
        Eigen::Vector3d p = Eigen::Vector3d(points.at(i).x(),  points.at(i).y(),  points.at(i).z());
        Eigen::Vector3d n = Eigen::Vector3d(normals.at(i).x(), normals.at(i).y(), normals.at(i).z());

        uint ii = 4*i;
        f(ii) = 0;
        f.template segment<3>(ii+1) = n;

        for(uint j=0; j<np; ++j)
        {
            uint jj = 4*j;
            Eigen::Vector3d diff = p-center.col(j);
            double len=diff.norm();
            if(len==0)
            {
                A.template block<4,4>(ii,jj).setZero();
            }
            else
            {
                double w    = RBF::eval_f(len);
                double dw_l = RBF::eval_df(len)/len;
                double ddw  = RBF::eval_ddf(len);
                Eigen::Vector3d g = diff*dw_l;
                A(ii,jj) = w;
                A.row(ii).template segment<3>(jj+1) = g.transpose();
                A.col(jj).template segment<3>(ii+1) = g;
                A.template block<3,3>(ii+1,jj+1)  = (ddw - dw_l)/(len*len) * (diff*diff.transpose());
                A.template block<3,3>(ii+1,jj+1).diagonal().array() += dw_l;
            }
        }
    }

    x = A.lu().solve(f);
    Eigen::Map<Eigen::Matrix4Xd> mx(x.data(), 4, np);

    alpha = mx.row(0);
    beta  = mx.template bottomRows<3>();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class RBF>
CINO_INLINE
ScalarField Hermite_RBF<RBF>::eval(const std::vector<vec3d> & plist) const
{
    ScalarField f(plist.size());
    for(uint i=0; i<plist.size(); ++i) f[i] = eval(plist.at(i));
    return f;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class RBF>
CINO_INLINE
double Hermite_RBF<RBF>::eval(const vec3d & p) const
{
    Eigen::Vector3d pp(p.x(), p.y(), p.z());
    double val = 0;
    for(uint i=0; i<center.cols(); ++i)
    {
        Eigen::Vector3d diff = pp-center.col(i);
        double l = diff.norm();
        if(l>0)
        {
            val += alpha(i) * RBF::eval_f(l);
            val += beta.col(i).dot(diff)*RBF::eval_df(l)/l;
        }
    }
    return val;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class RBF>
CINO_INLINE
vec3d Hermite_RBF<RBF>::eval_grad(const vec3d &p) const
{
    Eigen::Vector3d pp(p.x(), p.y(), p.z());
    Eigen::Vector3d grad = Eigen::Vector3d::Zero();

    for(uint i=0; i<center.cols(); i++)
    {
        Eigen::Vector3d node = center.col(i);
        Eigen::Vector3d beta = this->beta.col(i);
        Eigen::Vector3d diff = pp - node;

        float len=diff.norm();
        if(len>0.00001f)
        {
            Eigen::Vector3d diffNormalized = diff;
            diffNormalized.normalize();

            float dphi       = RBF::eval_df(len);
            float ddphi      = RBF::eval_ddf(len);
            float alpha_dphi = alpha(i) * dphi;
            float bDotd_l    = beta.dot(diff)/len;
            float squared_l  = diff.squaredNorm();

            grad += alpha_dphi * diffNormalized;
            grad += bDotd_l * (ddphi*diffNormalized - diff*dphi/squared_l) + beta*dphi/len ;
        }
    }

    return vec3d(grad[0], grad[1], grad[2]);
}

}
