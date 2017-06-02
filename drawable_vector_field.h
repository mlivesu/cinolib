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
#ifndef CINO_DRAWABLE_VECTOR_FIELD_H
#define CINO_DRAWABLE_VECTOR_FIELD_H

#include <cinolib/vector_field.h>
#include <cinolib/drawable_object.h>
#include <cinolib/colors.h>
#include <cinolib/gl/draw_arrow.h>

namespace cinolib
{

template <class Mesh>
class DrawableVectorField : public VectorField, public DrawableObject
{
    public:

        DrawableVectorField()
        {
            type = VECTOR_FIELD;
        }

        DrawableVectorField(const Mesh & m) : VectorField(m.num_elems())
        {
            type     = VECTOR_FIELD;
            m_ptr    = &m;
            color[0] = RED[0];
            color[1] = RED[1];
            color[2] = RED[2];

            set_arrow_size(0.5);
        }


        void draw(const float) const
        {
            if (m_ptr)
            {
                for(int tid=0; tid<m_ptr->num_elems(); ++tid)
                {
                    vec3d base = m_ptr->elem_centroid(tid);
                    vec3d tip  = base + arrow_length * vec_at(tid);

                    arrow<vec3d>(base, tip, arrow_thicknes, color);
                }
            }
        }

        vec3d scene_center() const { return vec3d(); }
        float scene_radius() const { return 0.0;     }

        inline void set_color(float r, float g, float b)
        {
            color[0] = r;
            color[1] = g;
            color[2] = b;
        }

        void set_arrow_size(float s)
        {
            if (m_ptr)
            {
                arrow_length   = m_ptr->edge_avg_length() * s;
                arrow_thicknes = arrow_length * 0.1;
            }
        }

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

        const Mesh *m_ptr;

        float arrow_length;
        float arrow_thicknes;
        float color[3];
};

}

#endif // CINO_DRAWABLE_VECTOR_FIELD_H
