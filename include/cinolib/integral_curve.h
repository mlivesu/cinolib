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
#ifndef CINO_INTEGRAL_CURVE_H
#define CINO_INTEGRAL_CURVE_H

#include <sys/types.h>
#include <vector>
#include <cinolib/drawable_curve.h>
#include <cinolib/vector_field.h>

namespace cinolib
{

template<class Mesh>
class IntegralCurve : public DrawableCurve
{

    public:

        explicit IntegralCurve(const Mesh                & m,
                               const VectorField         & grad,
                               const uint                  eid,
                               const std::vector<double> & bary);

        explicit IntegralCurve(const Mesh        & m,
                               const VectorField & grad,
                               const uint          vid);

        typedef enum
        {
            COMPUTING,
            END_ON_MAXIMA,
            END_ON_BORDER,
            INFINITE_LOOP,
        }
        Status;

        int exit_status() const { return status; }

    protected:

        Status status;

        const Mesh        * m_ptr;
        const VectorField * grad_ptr;


        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void trace_curve  (const Sample & seed);
        bool is_converged (const Sample & sample);

        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Curve::Sample move_forward             (const Sample & s);
        Curve::Sample move_forward_from_vertex (const uint vid) ;
        Curve::Sample move_forward_from_edge   (const uint eid, const vec3d & p);
        Curve::Sample move_forward_from_face   (const uint tid, const vec3d & p);
        Curve::Sample move_forward_from_face   (const uint tid, const uint fid, const vec3d & p);
        Curve::Sample move_forward_from_poly   (const uint pid, const vec3d & p);

        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool is_on_vertex (const Sample & s, uint & vid, const double tol = 1e-7) const;
        bool is_on_edge   (const Sample & s, uint & eid, const double tol = 1e-7) const;
        bool is_on_face   (const Sample & s, uint & tid, const double tol = 1e-7) const;
        bool is_on_face   (const Sample & s, uint & tid, uint & fid, const double tol = 1e-7) const;
        bool is_on_poly   (const Sample & s, uint & pid, const double tol = 1e-7) const;

        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Curve::Sample make_sample(const uint vid) const;
};

}

#ifndef  CINO_STATIC_LIB
#include "integral_curve.cpp"
#endif

#endif // CINO_INTEGRAL_CURVE_H
