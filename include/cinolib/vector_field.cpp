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
#include <cinolib/vector_field.h>
#include <fstream>

namespace cinolib
{

CINO_INLINE
VectorField::VectorField() {}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
VectorField::VectorField(const int size)
{
    setZero(3 * size);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
VectorField::VectorField(const std::vector<vec3d> & data)
{
    setZero(3 * data.size());
    for(uint i=0; i<data.size(); ++i) set(i,data.at(i));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d VectorField::vec_at(const int pos) const
{
    int ptr = pos * 3;
    return vec3d((*this)[ptr], (*this)[ptr+1], (*this)[ptr+2]);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void VectorField::set(const int pos, const vec3d & vec)
{
    int ptr = pos * 3;
    (*this)[ptr + 0] = vec.x();
    (*this)[ptr + 1] = vec.y();
    (*this)[ptr + 2] = vec.z();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void VectorField::normalize()
{
    /* IMPORTANT NOTE: for heatflow-based geodesic
     * distances there is a BIG difference between
     * this:
     *          v /= v.length()
     *
     * and this:
     *
     *          v.normalize()
     *
     * Here I choose the former. Play with it if you
     * need something different...
    */
    for(int i=0; i<rows(); i+=3)
    {
        vec3d tmp((*this)[i], (*this)[i+1], (*this)[i+2]);
        (*this)[i + 0] /= tmp.length();
        (*this)[i + 1] /= tmp.length();
        (*this)[i + 2] /= tmp.length();
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void VectorField::serialize(const char *filename) const
{
    std::ofstream f;
    f.precision(std::numeric_limits<double>::digits10+1);
    f.open(filename);
    assert(f.is_open());
    f << "VECTOR_FIELD " << size()/3 << "\n";
    for(int i=0; i<rows(); ++i)
    {
        f << (*this)[i] << "\n";
    }
    f.close();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void VectorField::deserialize(const char *filename)
{
    std::ifstream f;
    f.precision(std::numeric_limits<double>::digits10+1);
    f.open(filename);
    assert(f.is_open());
    uint size;
    std::string dummy;
    f >> dummy >> size;
    resize(size*3);
    for(uint i=0; i<3*size; ++i) f >> (*this)[i];
    f.close();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// for more info, see:
// http://eigen.tuxfamily.org/dox/TopicCustomizingEigen.html
//
// This method allows you to assign Eigen expressions to VectorField
//
template<typename OtherDerived>
CINO_INLINE
VectorField & VectorField::operator= (const Eigen::MatrixBase<OtherDerived>& other)
{
    this->Eigen::VectorXd::operator=(other);
    return *this;
}
//
// This constructor allows you to construct VectorField from Eigen expressions
//
template<typename OtherDerived>
CINO_INLINE
VectorField::VectorField(const Eigen::MatrixBase<OtherDerived>& other) : Eigen::VectorXd(other) {}

}

