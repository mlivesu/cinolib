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
#ifndef CURVE_H
#define CURVE_H

#include <cinolib/cinolib.h>
#include <cinolib/bbox.h>
#include <cinolib/meshes/skel/skel.h>
#include <cinolib/meshes/trimesh/trimesh.h>

namespace cinolib
{

class Curve
{
    public:

        // OPTIONAL - Connection to the Mesh
        // For each curve sample we associate an element id (tid)
        // and we express the position of the sample as a weighted
        // sum (barycentric coordinates) of its vertices.
        //
        // This is important to embed the curve in the mesh tessellation..
        //
        typedef struct
        {
            int                 tid  = -1;
            std::vector<double> bary = std::vector<double>();
        }
        MeshSample;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Curve();
        Curve(const std::vector<vec3d> & samples);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Skel             export_as_skeleton()    const;
        std::vector<int> tessellate(Trimesh & m) const;
        void             tessellate(Trimesh & m, const std::vector<int> & split_list) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const Bbox                    & bbox()      const;
        const std::vector<vec3d>      & samples()   const;
        const std::vector<MeshSample> & mesh_link() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        int    size() const;
        double length() const;
        void   append_sample(const vec3d & s);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // Arc-length parameterization (t \in [0,1])
        //
        vec3d sample_curve_at(const float t) const;
        vec3d sample_curve_at(const float t, const double tot_length) const;
        void  resample_curve(const int n_samples);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    protected:

        Bbox bb;
        std::vector<vec3d>      curve_samples;
        std::vector<MeshSample> link_to_mesh;
};

CINO_INLINE std::ostream & operator<<(std::ostream & in, const Curve & c);

}

#ifndef  CINO_STATIC_LIB
#include "curve.cpp"
#endif

#endif // CURVE_H
