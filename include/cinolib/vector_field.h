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
