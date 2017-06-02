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


namespace cinolib
{

CINO_INLINE
ScalarField::ScalarField()
{}

CINO_INLINE
ScalarField::ScalarField(const std::vector<float> & data)
{
    resize(data.size());
    for(int i=0; i<rows(); ++i)
    {
        (*this)[i] = data[i];
    }
}

CINO_INLINE
ScalarField::ScalarField(const int size)
{
    setZero(size);
}

CINO_INLINE
ScalarField::ScalarField(const char *filename)
{
    deserialize(filename);
}


template<class Mesh>
CINO_INLINE
void ScalarField::copy_to_mesh(Mesh & m) const
{
    assert(rows() == m.num_verts());
    for(int vid=0; vid<rows(); ++vid)
    {
        m.vert_data(vid).uvw[0] = (*this)[vid];
    }
}


CINO_INLINE
void ScalarField::normalize_in_01()
{
    double min =  FLT_MAX;
    double max = -FLT_MAX;

    for(int i=0; i<this->rows(); ++i)
    {
        min = std::min(min, (*this)[i]);
        max = std::max(max, (*this)[i]);
    }

    logger << endl;
    logger << "Min value " << min << endl;
    logger << "Max value " << max << endl;
    logger << "Normalized in the range [0,1]" << endl;
    logger << endl;

    double delta = max - min;

    for(int i=0; i<rows(); ++i)
    {
        (*this)[i] = ((*this)[i] - min) / delta;
    }
}


CINO_INLINE
int ScalarField::min_element_index() const
{
    double min_val =  FLT_MAX;
    int    min_el  = -1;

    for(int i=0; i<rows(); ++i)
    {
        if (min_val > (*this)[i])
        {
            min_val = std::min(min_val, (*this)[i]);
            min_el  = i;
        }
    }
    assert(min_el > -1);
    return min_el;
}


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


CINO_INLINE
void ScalarField::deserialize(const char *filename)
{
    std::ifstream f;
    f.precision(std::numeric_limits<double>::digits10+1);
    f.open(filename);
    assert(f.is_open());
    int size;
    std::string dummy;
    f >> dummy >> size;
    resize(size);
    for(int i=0; i<size; ++i) f >> (*this)[i];
    f.close();
}

}
