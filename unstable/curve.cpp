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
#include <cinolib/unstable/curve.h>

namespace cinolib
{

CINO_INLINE
Curve::Curve()
{}


CINO_INLINE
Curve::Curve(const std::vector<vec3d> & samples)
{
    this->curve_samples = samples;
}


CINO_INLINE
const Bbox & Curve::bbox() const
{
    return bb;
}


CINO_INLINE
const std::vector<vec3d> &Curve::samples() const
{
    return curve_samples;
}


CINO_INLINE
double Curve::length() const
{
    double l = 0.0;
    for(size_t i=1; i<curve_samples.size(); ++i)
    {
        l += curve_samples.at(i).dist(curve_samples.at(i-1));
    }
    return l;
}

CINO_INLINE
void Curve::append_sample(const vec3d & s)
{
    curve_samples.push_back(s);
    bb.min = bb.min.min(s);
    bb.max = bb.max.max(s);
}


CINO_INLINE
vec3d Curve::sample_curve_at(const float t) const
{
    return sample_curve_at(t, length());
}


CINO_INLINE
vec3d Curve::sample_curve_at(const float t, const double tot_length) const
{
    assert(t>=0.0);
    assert(t<=1.0);

    double curr_l = 0.0;
    double curr_t = 0.0;
    for(size_t i=1; i<curve_samples.size(); ++i)
    {
        double seg_l   = curve_samples[i-1].dist(curve_samples[i]);
        double delta_t = seg_l / tot_length;

        if (curr_t + delta_t >= t)
        {
            double alpha = (t-curr_t)/(delta_t);
            return (1.0-alpha)*curve_samples[i-1] + alpha*curve_samples[i];
        }
        else
        {
            curr_t += delta_t;
            curr_l += seg_l;
        }
    }
}

CINO_INLINE
void Curve::resample_curve(const int n_samples)
{
    std::vector<vec3d> new_samples;

    double tot_length = length();
    double delta_t    = 1.0/double(n_samples);
    double t          = 0.0;

    for(int i=0; i<n_samples; ++i)
    {
        new_samples.push_back( sample_curve_at(t,tot_length) );
        t += delta_t;
    }

    curve_samples = new_samples;
}

}
