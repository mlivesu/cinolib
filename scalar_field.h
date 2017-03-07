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
#ifndef SCALAR_FIELD_V_H
#define SCALAR_FIELD_V_H

#include <cinolib/cinolib.h>
#include <cinolib/serializable.h>

#include <iostream>
#include <float.h>
#include <vector>
#include <eigen3/Eigen/Dense>

namespace cinolib
{

class ScalarField : public Eigen::VectorXd, public Serializable
{
    public:

        ScalarField();
        ScalarField(const std::vector<float> & data);
        ScalarField(const int size);
        ScalarField(const char *filename);

        template<class Mesh>
        void copy_to_mesh(Mesh & m) const;

        void normalize_in_01();
        int  min_element_index() const;

        void serialize  (const char *filename) const;
        void deserialize(const char *filename);
};

}

#ifndef  CINO_STATIC_LIB
#include "scalar_field.cpp"
#endif

#endif // SCALAR_FIELD_V_H
