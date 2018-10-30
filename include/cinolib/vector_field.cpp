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
#include <cinolib/vector_field.h>
#include <fstream>

namespace cinolib
{

CINO_INLINE
VectorField::VectorField() {}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
VectorField::VectorField(const uint size)
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

