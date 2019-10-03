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
#ifndef CINO_CURVE_H
#define CINO_CURVE_H

#include <sys/types.h>
#include <vector>
#include <iostream>
#include <cinolib/geometry/aabb.h>
#include <cinolib/cino_inline.h>
#include <cinolib/ipair.h>
#include <cinolib/meshes/skel.h>
#include <cinolib/meshes/trimesh.h>

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
            // a mesh element (e.g. a triangle or tetrahedron). This can be
            // important for some algorithm, for example to embed the curve
            // mesh tessellation..
            //
            int                 pid  = -1;
            std::vector<double> bary = std::vector<double>();            
        }
        Sample;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit Curve();
        explicit Curve(const std::vector<vec3d> & samples);
        explicit Curve(const Skel & skel, const uint bone);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void reverse();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint                        size()    const;
        double                      length()  const;
        const std::vector<Sample> & samples() const;
              std::vector<Sample> & samples();
        const AABB                & bbox()    const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void append_sample(const vec3d  & s);
        void append_sample(const Sample & s);
        void pop_back();
        void pop_front(); // inefficient! - do it properly

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // Arc-length parameterization (t \in [0,1])
        //
        void              update_arc_length_param(); // recomputes parameter t for each sample
        uint              last_sample_lower_equal_than(const float t) const; // returns index
        uint              sample_closest_to(const float t) const; // reteurns index
        vec3d             sample_curve_at(const float t) const;
        vec3d             sample_curve_at(const float t, const double tot_length) const;
        void              resample_curve(const uint n_samples);
        std::vector<uint> select_n_samples(const uint n_samples) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Skel                export_as_skeleton() const;
        std::vector<double> vector_coords()      const;
        std::vector<int>    vector_segments()    const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void operator+=(const Curve & c);

    private:

        AABB bb;
        std::vector<Sample> sample_list;
};

CINO_INLINE std::ostream & operator<<(std::ostream & in, const Curve::Sample & s);
CINO_INLINE std::ostream & operator<<(std::ostream & in, const Curve         & c);

}

#ifndef  CINO_STATIC_LIB
#include "curve.cpp"
#endif

#endif // CINO_CURVE_H
