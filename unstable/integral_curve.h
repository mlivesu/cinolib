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

#include <cinolib/cinolib.h>
#include <cinolib/vector_field.h>
#include <cinolib/unstable/drawable_curve.h>

namespace cinolib
{

template<class Mesh>
class IntegralCurve : public DrawableCurve
{
    enum
    {
        STOP_AT_LOCAL_MAX,
        STOP_AT_GIVEN_VTX,
        STOP_AT_GIVEN_VAL,
    };

    typedef struct
    {
        int   convergence_criterion = STOP_AT_LOCAL_MAX;
        int   source_tid            = -1;
        int   source_vid            = -1;
        vec3d source_pos            = vec3d();
        float stop_at_this_value    = -1;
        float stop_at_this_vertex   = -1;
    } Options;

    typedef struct
    {
        vec3d pos     = vec3d(0,0,0);
        int   elem_id = -1; // element (triangle, tet,...)
        int   gate_id = -1; // gate (edge, tet facet, ...)
        int   vert_id = -1; // vertex
    } CurveSample;

    public:

        IntegralCurve(const Mesh        & m,
                      const VectorField & grad,
                      const int           source_tid,
                      const vec3d         start_pos);

        IntegralCurve(const Mesh        & m,
                      const VectorField & grad,
                      const int           source_tid,
                      const int           source_vid);

        IntegralCurve(const Mesh        & m,
                      const VectorField & grad,
                      const int           source_tid,
                      const int           source_vid,
                      const float         stop_at_this_value);

        IntegralCurve(const Mesh        & m,
                      const VectorField & grad,
                      const int           source_tid,
                      const int           source_vid,
                      const int           stop_at_this_vertex);

    private:

        void make_curve();

        bool is_converged(const int curr_tid, const int convergence_criterion) const;

        void find_exit_gate(const CurveSample & sample,
                            const vec3d       & dir,
                                  int         & exit_gate,
                                  vec3d       & exit_pos) const;

        CurveSample traverse_element(const CurveSample & curr);

        void traverse_element(const CurveSample & curr_sample,
                                    CurveSample & next_sample) const;

        bool gradient_skins_into(const CurveSample & curr,
                                 const CurveSample & next) const;

    private:

        Options opt;

        std::vector<CurveSample> curve;

        const Mesh        * m_ptr;
        const VectorField * grad_ptr;
};

}

#ifndef  CINO_STATIC_LIB
#include "integral_curve.cpp"
#endif

#endif // INTEGRAL_CURVE_H
