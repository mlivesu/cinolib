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
#ifndef INTEGRAL_CURVE_H
#define INTEGRAL_CURVE_H

#include "cinolib.h"
#include "vector_field.h"
#include "drawable_object.h"

namespace cinolib
{

template<class Mesh>
class IntegralCurve : public DrawableObject
{
    public:

        IntegralCurve(const Mesh        & m,
                      const VectorField & grad,
                      const int           source_tid,
                      const vec3d       & source_pos);

        IntegralCurve(const Mesh        & m,
                      const VectorField & grad,
                      const int           source_tid,
                      const vec3d       & source_pos,
                      const float         stop_at_this_value);

        IntegralCurve(const Mesh        & m,
                      const VectorField & grad,
                      const int           source_tid,
                      const vec3d       & source_pos,
                      const int           stop_at_this_vertex);

        vec3d scene_center() const { return m_ptr->bbox().center(); }
        float scene_radius() const { return m_ptr->bbox().diag();   }
        void  draw() const;


    protected:

        void find_exit_door(const int     tid,
                            const vec3d & pos,
                            const vec3d & target_dir,
                                  int   & exit_edge,
                                  int   & exit_backup) const;


        void traverse_element(const int     tid,
                              const vec3d & pos,
                                    int   & next_tid,
                                    vec3d & next_pos) const;


        void handle_corner_cases(const int     curr_tid,
                                 const vec3d & A,
                                 const vec3d & B,
                                 const int     exit_backup,
                                       int   & next_tid,
                                       vec3d & next_pos) const;


    private:

        std::vector<vec3d>  curve;
        const Mesh        * m_ptr;
        const VectorField * grad_ptr;
};

}

#ifndef  CINO_STATIC_LIB
#include "integral_curve.cpp"
#endif

#endif // INTEGRAL_CURVE_H
