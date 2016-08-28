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

    public:

        IntegralCurve(const Mesh                & m,
                      const VectorField         & grad,
                      const int                   tid,
                      const std::vector<double> & bary);

        IntegralCurve(const Mesh        & m,
                      const VectorField & grad,
                      const int           vid);

    protected:

        const Mesh        * m_ptr;
        const VectorField * grad_ptr;

        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void trace_curve  (const Sample & seed);
        bool is_converged (const Sample & sample);

        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Curve::Sample move_forward             (const Sample & s);
        Curve::Sample move_forward_from_vertex (const int vid) ;
        Curve::Sample move_forward_from_edge   (const int eid, const vec3d & p);
        Curve::Sample move_forward_from_face   (const int tid, const vec3d & p);
        Curve::Sample move_forward_from_face   (const int tid, const int fid, const vec3d & p);
        Curve::Sample move_forward_from_cell   (const int cid, const vec3d & p);

        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool is_on_vertex (const Sample & s, int & vid, const double tol = 1e-7) const;
        bool is_on_edge   (const Sample & s, int & eid, const double tol = 1e-7) const;
        bool is_on_face   (const Sample & s, int & tid, const double tol = 1e-7) const;
        bool is_on_face   (const Sample & s, int & tid, int & fid, const double tol = 1e-7) const;
        bool is_on_cell   (const Sample & s, int & cid, const double tol = 1e-7) const;
};

}

#ifndef  CINO_STATIC_LIB
#include "integral_curve.cpp"
#endif

#endif // INTEGRAL_CURVE_H
