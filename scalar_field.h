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
#ifndef CINO_SCALAR_FIELD_V_H
#define CINO_SCALAR_FIELD_V_H

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
        ScalarField(const uint size);
        ScalarField(const char *filename);

        template<class Mesh>
        void copy_to_mesh(Mesh & m) const;

        void normalize_in_01();
        uint min_element_index() const;

        void serialize  (const char *filename) const;
        void deserialize(const char *filename);
};

}

#ifndef  CINO_STATIC_LIB
#include "scalar_field.cpp"
#endif

#endif // CINO_SCALAR_FIELD_V_H
