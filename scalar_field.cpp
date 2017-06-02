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
