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
#include <cinolib/scalar_field.h>
#include <cinolib/cino_inline.h>
#include <cinolib/inf.h>
#include <fstream>

namespace cinolib
{

CINO_INLINE
ScalarField::ScalarField()
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
ScalarField::ScalarField(const std::vector<double> &data)
{
    resize(data.size());
    for(int i=0; i<rows(); ++i)
    {
        (*this)[i] = data[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
ScalarField::ScalarField(const uint size)
{
    setZero(size);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
ScalarField::ScalarField(const char *filename)
{
    deserialize(filename);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void ScalarField::copy_to_mesh(Mesh & m, const int tex_coord) const
{
    assert(rows() == m.num_verts());
    for(uint vid=0; vid<rows(); ++vid)
    {
        switch (tex_coord)
        {
            case U_param : m.vert_data(vid).uvw[0] = (*this)[vid]; break;
            case V_param : m.vert_data(vid).uvw[1] = (*this)[vid]; break;
            case W_param : m.vert_data(vid).uvw[2] = (*this)[vid]; break; break;
            default: assert(false);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void ScalarField::normalize_in_01()
{
    double min =  inf_double;
    double max = -inf_double;

    for(int i=0; i<this->rows(); ++i)
    {
        min = std::min(min, (*this)[i]);
        max = std::max(max, (*this)[i]);
    }

    std::cout << std::endl;
    std::cout << "Min value " << min << std::endl;
    std::cout << "Max value " << max << std::endl;
    std::cout << "Normalized in the range [0,1]" << std::endl;
    std::cout << std::endl;

    double delta = max - min;

    for(int i=0; i<rows(); ++i)
    {
        (*this)[i] = ((*this)[i] - min) / delta;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint ScalarField::min_element_index() const
{
    double min_val = (*this)[0];
    uint   min_el  = 0;

    for(int i=1; i<rows(); ++i)
    {
        if (min_val > (*this)[i])
        {
            min_val = std::min(min_val, (*this)[i]);
            min_el  = i;
        }
    }
    return min_el;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void ScalarField::serialize(const char *filename) const
{
    std::ofstream f;
    f.precision(std::numeric_limits<double>::digits10+1);
    f.open(filename);
    assert(f.is_open());
    f << "SCALAR_FIELD " << size() << "\n";
    for(int i=0; i<rows(); ++i)
    {
        f << (*this)[i] << "\n";
    }
    f.close();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void ScalarField::deserialize(const char *filename)
{
    std::ifstream f;
    f.precision(std::numeric_limits<double>::digits10+1);
    f.open(filename);
    assert(f.is_open());
    uint size;
    std::string dummy;
    f >> dummy >> size;
    resize(size);
    for(uint i=0; i<size; ++i) f >> (*this)[i];
    f.close();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// for more info, see:
// http://eigen.tuxfamily.org/dox/TopicCustomizingEigen.html
//
// This method allows you to assign Eigen expressions to ScalarField
//
template<typename OtherDerived>
CINO_INLINE
ScalarField & ScalarField::operator= (const Eigen::MatrixBase<OtherDerived>& other)
{
    this->Eigen::VectorXd::operator=(other);
    return *this;
}
//
// This constructor allows you to construct ScalarField from Eigen expressions
//
template<typename OtherDerived>
CINO_INLINE
ScalarField::ScalarField(const Eigen::MatrixBase<OtherDerived>& other) : Eigen::VectorXd(other) {}

}
