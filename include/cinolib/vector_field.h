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
#ifndef CINO_VECTOR_FIELD_H
#define CINO_VECTOR_FIELD_H

#include <cinolib/geometry/vec3.h>
#include <cinolib/serializable.h>
#include <Eigen/Dense>

namespace cinolib
{

class VectorField : public Eigen::VectorXd, public Serializable
{
    public:

        explicit VectorField();
        explicit VectorField(const uint size);
        explicit VectorField(const std::vector<vec3d> & data);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d vec_at(const int pos) const;
        void  set(const int pos, const vec3d & vec);
        void  normalize();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void serialize  (const char *filename) const;
        void deserialize(const char *filename);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // for more info, see:
        // http://eigen.tuxfamily.org/dox/TopicCustomizingEigen.html
        //
        // This method allows you to assign Eigen expressions to VectorField
        //
        template<typename OtherDerived>
        VectorField & operator= (const Eigen::MatrixBase<OtherDerived>& other);

        //
        // This constructor allows you to construct VectorField from Eigen expressions
        //
        template<typename OtherDerived>
        VectorField(const Eigen::MatrixBase<OtherDerived>& other);
};

}

#ifndef  CINO_STATIC_LIB
#include "vector_field.cpp"
#endif


#endif // CINO_VECTOR_FIELD_H
