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
#ifndef DRAWABLE_VECTOR_FIELD_H
#define DRAWABLE_VECTOR_FIELD_H

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

        DrawableVectorField(const Mesh & m) : VectorField(m.num_elements())
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
                for(int tid=0; tid<m_ptr->num_elements(); ++tid)
                {
                    vec3d base = m_ptr->element_barycenter(tid);
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
                arrow_length   = m_ptr->avg_edge_length() * s;
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

#endif // DRAWABLE_VECTOR_FIELD_H
