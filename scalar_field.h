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

#include <iostream>
#include <float.h>
#include <vector>
#include <eigen3/Eigen/Dense>

namespace cinolib
{

class ScalarField : public Eigen::VectorXd
{
    public:

        ScalarField(){}
        ScalarField(const std::vector<float> & data)
        {
            resize(data.size());
            for(int i=0; i<rows(); ++i)
            {
                (*this)[i] = data[i];
            }
        }

        ScalarField(const int size)
        {
            setZero(size);
        }

        template<class Mesh>
        void copy_to_mesh(Mesh & m) const
        {
            assert(rows() == m.num_vertices());
            for(int vid=0; vid<rows(); ++vid)
            {
                m.set_vertex_u_text(vid, (*this)[vid]);
            }
        }

        void normalize_in_01()
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

        int min_element() const
        {
            double min_val =  FLT_MAX;
            int    min_el  = -1;

            for(int i=0; i<this->rows(); ++i)
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
};

}

#endif // SCALAR_FIELD_V_H
