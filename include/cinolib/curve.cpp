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
#include <cinolib/curve.h>
#include <cinolib/min_max_inf.h>

namespace cinolib
{

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Curve::Sample & s)
{
    in << s.pos <<  " \t " << s.pid << " \t ";
    for(double w : s.bary) in << w << "  ";
    in << std::endl;
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Curve & c)
{
    for(Curve::Sample s : c.samples()) in << s;
    in << std::endl;
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Curve::Curve()
{}


CINO_INLINE
Curve::Curve(const std::vector<vec3d> & samples)
{
    for(vec3d p : samples) append_sample(p);
    update_arc_length_param();
}

CINO_INLINE
Curve::Curve(const Skel & skel, const uint bone)
{
    std::vector<vec3d> samples;
    for(int vid : skel.vertex_bone(bone))
    {
        samples.push_back(skel.vertex(vid));
    }
    for(vec3d p : samples) append_sample(p);
    update_arc_length_param();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::reverse()
{
    std::reverse(sample_list.begin(), sample_list.end());
    update_arc_length_param();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Skel Curve::export_as_skeleton() const
{
    Skel s(vector_coords(), vector_segments());
    return s;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<double> Curve::vector_coords() const
{
    std::vector<double> coords;
    for(uint i=0; i<sample_list.size(); ++i)
    {
        coords.push_back(sample_list.at(i).pos.x());
        coords.push_back(sample_list.at(i).pos.y());
        coords.push_back(sample_list.at(i).pos.z());
    }
    return coords;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<int> Curve::vector_segments() const
{
    std::vector<int> segs;
    for(uint i=1; i<sample_list.size(); ++i)
    {
        segs.push_back(i-1);
        segs.push_back( i );
    }
    return segs;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::operator+=(const Curve & c)
{
    for(Sample s : c.samples()) append_sample(s);
    update_arc_length_param();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
const AABB & Curve::bbox() const
{
    return bb;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
const std::vector<Curve::Sample> & Curve::samples() const
{
    return sample_list;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<Curve::Sample> & Curve::samples()
{
    return sample_list;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint Curve::size() const
{
    return sample_list.size();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Curve::length() const
{
    double l = 0.0;
    for(uint i=1; i<sample_list.size(); ++i)
    {
        l += sample_list.at(i).pos.dist(sample_list.at(i-1).pos);
    }
    return l;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::append_sample(const vec3d & s)
{
    Sample sample;
    sample.pos = s;
    append_sample(sample);
    update_arc_length_param();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::append_sample(const Sample & s)
{
    sample_list.push_back(s);
    bb.min = bb.min.min(s.pos);
    bb.max = bb.max.max(s.pos);
    update_arc_length_param();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::pop_back()
{
    sample_list.pop_back();
    update_arc_length_param();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::pop_front() // inefficient! - do it properly
{
    reverse();
    pop_back();
    reverse();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::update_arc_length_param() // recomputes parameter t for each sample
{
    if (size() < 2) return;

    double tot_length = length();
    double curr_l     = 0.0;
    double curr_t     = 0.0;

    for(uint i=1; i<sample_list.size()-1; ++i)
    {
        double seg_l   = sample_list.at(i-1).pos.dist(sample_list.at(i).pos);
        double delta_t = seg_l / tot_length;

        curr_t += delta_t;
        curr_l += seg_l;

        sample_list.at(i).t = curr_t;
    }

    sample_list.front().t = 0.0;
    sample_list.back().t  = 1.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint Curve::last_sample_lower_equal_than(const float t) const
{
    assert(t>=0);
    assert(t<=1);

    for(size_t i=1; i<sample_list.size()-1; ++i)
    {
        if (sample_list.at(i).t > t) return i-1;
    }

    return sample_list.size()-1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint Curve::sample_closest_to(const float t) const
{
    assert(t>=0);
    assert(t<=1);

    float best_err = inf_float;
    uint  best_sam = 0;

    for(uint pos=0; pos<sample_list.size()-1; ++pos)
    {
        float err = fabs(sample_list.at(pos).t - t);
        if (err < best_err)
        {
            best_err = err;
            best_sam = pos;
        }
        else if (sample_list.at(pos).t > t) return best_sam;
    }

    return sample_list.size()-1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d Curve::sample_curve_at(const float t) const
{
    return sample_curve_at(t, length());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d Curve::sample_curve_at(const float t, const double tot_length) const
{
    assert(size() > 1);
    assert(tot_length > 0);
    assert(t>=0.0);
    assert(t<=1.0);

    double curr_l = 0.0;
    double curr_t = 0.0;
    for(uint i=1; i<sample_list.size(); ++i)
    {
        double seg_l   = sample_list.at(i-1).pos.dist(sample_list.at(i).pos);
        double delta_t = seg_l / tot_length;

        if (delta_t == 0) continue;

        if (curr_t + delta_t >= t - 1e-7)
        {
            double alpha = (t-curr_t)/(delta_t);
            return (1.0-alpha)*sample_list.at(i-1).pos + alpha*sample_list.at(i).pos;
        }
        else
        {
            curr_t += delta_t;
            curr_l += seg_l;
        }
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::resample_curve(const uint n_samples)
{
    assert(n_samples >= 2);

    double tot_length = length();
    double delta_t    = 1.0/double(n_samples-1);
    double t          = 0.0;

    if (size()  < 2) return;
    if (length()<=0) return;

    std::vector<Sample> new_samples;
    for(uint i=0; i<n_samples; ++i)
    {
        Sample s;
        s.pos = sample_curve_at(t,tot_length);
        new_samples.push_back(s);
        t += delta_t;
    }
    sample_list = new_samples;

    assert(new_samples.size() == n_samples);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<uint> Curve::select_n_samples(const uint n_samples) const
{
    assert(size() >= n_samples);

    float i    = 0;
    float step = float(size()-1) / float(n_samples-1);

    std::vector<uint> list;

    do
    {
        uint new_pos = std::ceil(i);
        if (new_pos == samples().size()) --new_pos; // this may happen due to roundoff errors...
        list.push_back(new_pos);
        i += step;
    }
    while (list.size() < n_samples);

    return list;
}

}
