/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#include <cinolib/vector_field.h>

namespace cinolib
{

CINO_INLINE
VectorField::VectorField() {}

CINO_INLINE
VectorField::VectorField(const int size)
{
    setZero(3 * size);
}

CINO_INLINE
vec3d VectorField::vec_at(const int pos) const
{
    int ptr = pos * 3;
    return vec3d((*this)[ptr], (*this)[ptr+1], (*this)[ptr+2]);
}

CINO_INLINE
void VectorField::set(const int pos, const vec3d & vec)
{
    int ptr = pos * 3;
    (*this)[ptr + 0] = vec.x();
    (*this)[ptr + 1] = vec.y();
    (*this)[ptr + 2] = vec.z();
}

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

