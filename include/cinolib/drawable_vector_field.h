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
#ifndef CINO_DRAWABLE_VECTOR_FIELD_H
#define CINO_DRAWABLE_VECTOR_FIELD_H

#include <cinolib/vector_field.h>
#include <cinolib/drawable_object.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/meshes/abstract_mesh.h>
#include <cinolib/color.h>
#include <cinolib/gl/draw_arrow.h>

namespace cinolib
{

class DrawableVectorField : public VectorField, public DrawableObject
{
    public:

        explicit DrawableVectorField();

        explicit DrawableVectorField(const uint size);

        explicit DrawableVectorField(const std::vector<vec3d> & data,
                                     const std::vector<vec3d> & pos);

        template<class M, class V, class E, class P>
        explicit DrawableVectorField(const AbstractMesh<M,V,E,P> & m,
                                     const bool field_on_poly = true);

        ~DrawableVectorField(){}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        ObjectType object_type() const { return DRAWABLE_VECTOR_FIELD; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void draw(const float) const;
        vec3d scene_center() const { return vec3d(); }
        float scene_radius() const { return 0.0;     }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void  set_arrow_color(const Color & c);
        void  set_arrow_size (float s);
        Color get_arrow_color() const;
        float get_arrow_size () const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // for more info, see:
        // http://eigen.tuxfamily.org/dox/TopicCustomizingEigen.html
        //
        // This method allows you to assign Eigen expressions to MyVectorType
        //
        template<typename OtherDerived>
        DrawableVectorField & operator= (const Eigen::MatrixBase<OtherDerived>& other)
        {
            this->Eigen::VectorXd::operator=(other);
            return *this;
        }
        //
        // This constructor allows you to construct MyVectorType from Eigen expressions
        //
        template<typename OtherDerived>
        DrawableVectorField(const Eigen::MatrixBase<OtherDerived>& other) : Eigen::VectorXd(other) {}

    private:

        std::vector<vec3d> pos; // application points for each element in the field
        float arrow_size;
        Color arrow_color;
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_vector_field.cpp"
#endif

#endif // CINO_DRAWABLE_VECTOR_FIELD_H
