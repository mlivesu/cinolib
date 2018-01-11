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
#ifndef CINO_COMMON_H
#define CINO_COMMON_H

#include <assert.h>
#include <cmath>
#include <vector>
#include <Eigen/Dense>

#include <cinolib/cinolib.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

typedef std::pair<uint,uint> ipair;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// http://stackoverflow.com/questions/3738384/stable-cotangent
//
CINO_INLINE
double cot(double x)
{
    return tan(M_PI_2 - x);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
ipair unique_pair(uint v0, uint v1)
{
    ipair e;
    e.first  = std::min(v0,v1);
    e.second = std::max(v0,v1);
    return e;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const ipair & p)
{
    in << "(" << p.first << "," << p.second << ") ";
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#ifndef NDEBUG
template<typename C>
CINO_INLINE
void CHECK_BOUNDS(const C & container, int index)
{
    assert(index >= 0);
    assert(index < (int)container.size());
}
#else
template<typename C>
CINO_INLINE void CHECK_BOUNDS(const C &, int) {}
#endif

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C,typename E>
CINO_INLINE
bool CONTAINS(const C & container, const E & element)
{
    return (container.find(element) != container.end());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C,typename E>
CINO_INLINE
bool DOES_NOT_CONTAIN(const C & container, const E & element)
{
    return !CONTAINS(container,element);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C,typename E>
CINO_INLINE
bool CONTAINS_VEC(const C & container, const E & element)
{
    return (std::find(container.begin(), container.end(), element) != container.end());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C,typename E>
CINO_INLINE
bool DOES_NOT_CONTAIN_VEC(const C & container, const E & element)
{
    return !CONTAINS_VEC(container,element);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void REMOVE_FROM_VEC(std::vector<T> & vec, const T & elem)
{
    // https://en.wikipedia.org/wiki/Erase–remove_idiom
    //
    // IMPORTANT: erase–remove can only be used with containers
    // holding elements with full value semantics without incurring
    // resource leaks.
    //
    vec.erase(std::remove(vec.begin(), vec.end(), elem), vec.end());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void from_std_3x3_to_Eigen_3x3(const double stdM[3][3], Eigen::Matrix3d & eigenM)
{
    eigenM.coeffRef(0,0) = stdM[0][0];  eigenM.coeffRef(0,1) = stdM[0][1];  eigenM.coeffRef(0,2) = stdM[0][2];
    eigenM.coeffRef(1,0) = stdM[1][0];  eigenM.coeffRef(1,1) = stdM[1][1];  eigenM.coeffRef(1,2) = stdM[1][2];
    eigenM.coeffRef(2,0) = stdM[2][0];  eigenM.coeffRef(2,1) = stdM[2][1];  eigenM.coeffRef(2,2) = stdM[2][2];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void from_eigen_3x3_to_std_3x3(const Eigen::Matrix3d & eigenM, double stdM[3][3])
{
    stdM[0][0] = eigenM.coeffRef(0,0);  stdM[0][1] = eigenM.coeffRef(0,1);  stdM[0][2] = eigenM.coeffRef(0,2);
    stdM[1][0] = eigenM.coeffRef(1,0);  stdM[1][1] = eigenM.coeffRef(1,1);  stdM[1][2] = eigenM.coeffRef(1,2);
    stdM[2][0] = eigenM.coeffRef(2,0);  stdM[2][1] = eigenM.coeffRef(2,1);  stdM[2][2] = eigenM.coeffRef(2,2);
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
int serialize_2D_index(const int r, const int c, const int n_cols)
{
    return r * n_cols + c;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
ipair deserialize_2D_index(const int i, const int n_cols)
{
    return std::make_pair(i/n_cols, i%n_cols);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<vec3d> vec3d_from_serialized_xyz(const std::vector<double> & coords)
{
    uint nv = coords.size()/3;
    std::vector<vec3d> tmp(nv);
    for(uint vid=0; vid<nv; ++vid) tmp.at(vid) = vec3d(coords[3*vid+0], coords[3*vid+1], coords[3*vid+2]);
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<double> serialized_xyz_from_vec3d(const std::vector<vec3d> & verts)
{
    uint nv = verts.size();
    std::vector<double> tmp(3*nv);
    for(uint vid=0; vid<nv; ++vid)
    {
        tmp.at(3*vid  ) = verts.at(vid).x();
        tmp.at(3*vid+1) = verts.at(vid).y();
        tmp.at(3*vid+2) = verts.at(vid).z();
    }
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<std::vector<uint>> faces_from_serialized_vids(const std::vector<uint> & vids, const uint vids_per_face)
{
    uint nf = vids.size()/vids_per_face;
    std::vector<std::vector<uint>> tmp(nf);
    for(uint fid=0; fid<nf; ++fid)
    {
        for(uint off=0; off<vids_per_face; ++off) tmp.at(fid).push_back(vids.at(fid*vids_per_face+off));
    }
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<uint> serialized_vids_from_faces(const std::vector<std::vector<uint>> & faces)
{
    std::vector<uint> tmp;
    for(const auto & f   : faces)
    for(const auto & vid : f    )
    {
        tmp.push_back(vid);
    }
    return tmp;
}

}

#endif // CINO_COMMON_H
