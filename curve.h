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
#ifndef CINO_CURVE_H
#define CINO_CURVE_H

#include <cinolib/cinolib.h>
#include <cinolib/bbox.h>
#include <cinolib/meshes/skel/skel.h>
#include <cinolib/meshes/trimesh/trimesh.h>

namespace cinolib
{

class Curve
{
    public:

        typedef struct
        {
            vec3d pos = vec3d(0,0,0);
            float t   = -1; // arc-length param (t=0 => begin, t=1 => end)

            // OPTIONAL - Connection to a Mesh
            // Express the sample in terms of barycentric coordinates wrt to
            // a mesh element (either triangle or tetrahedral). This can be
            // important for some algorithm, for example to embed the curve
            // mesh tessellation..
            //
            int                 tid  = -1;
            std::vector<double> bary = std::vector<double>();            
        }
        Sample;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Curve();
        Curve(const std::vector<vec3d> & samples);
        Curve(const Skel & skel, const int bone);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void reverse();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        int                         size()    const;
        double                      length()  const;
        const std::vector<Sample> & samples() const;
              std::vector<Sample> & samples();
        const Bbox                & bbox()    const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void append_sample(const vec3d  & s);
        void append_sample(const Sample & s);
        void pop_back();
        void pop_front(); // inefficient! - do it properly

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // Arc-length parameterization (t \in [0,1])
        //
        void             update_arc_length_param(); // recomputes parameter t for each sample
        int              last_sample_lower_equal_than(const float t) const; // reteurns index
        int              sample_closest_to(const float t) const; // reteurns index
        vec3d            sample_curve_at(const float t) const;
        vec3d            sample_curve_at(const float t, const double tot_length) const;
        void             resample_curve(const int n_samples);
        std::vector<int> select_n_samples(const int n_samples) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::vector<int> tessellate(Trimesh & m) const;
        void             tessellate(Trimesh & m, const std::vector<int> & split_list) const;

        void split_in_2(const Trimesh            & m,
                        const int                  tid,
                        const std::pair<int,int> & edg,
                              std::vector<int>   & split_list) const;

        void split_in_3(const Trimesh            & m,
                        const int                  tid,
                        const std::pair<int,int> & edg0,
                        const std::pair<int,int> & edg1,
                              std::vector<int>   & split_list) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Skel                export_as_skeleton() const;
        std::vector<double> vector_coords()      const;
        std::vector<int>    vector_segments()    const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void operator+=(const Curve & c);

    private:

        Bbox bb;
        std::vector<Sample> sample_list;
};

CINO_INLINE std::ostream & operator<<(std::ostream & in, const Curve::Sample & s);
CINO_INLINE std::ostream & operator<<(std::ostream & in, const Curve         & c);

}

#ifndef  CINO_STATIC_LIB
#include "curve.cpp"
#endif

#endif // CINO_CURVE_H
