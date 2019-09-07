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
#include <cinolib/Poisson_sampling.h>
#include <cinolib/random_generator.h>
#include <cinolib/serialize_index.h>
#include <cinolib/min_max_inf.h>

namespace cinolib
{

/* This is almost a 1:1 copy of the reference implementation of the Fast Poisson disk sampling in arbitrary
 * dimensions released by Robert Bridson (https://www.cs.ubc.ca/~rbridson/download/curlnoise.tar.gz). See:
 *
 * Fast Poisson Disk Sampling in Arbitrary Dimensions
 * Robert Bridson
 * SIGGRAPH Technical Sketch, 2007
*/

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint Dim, class Point>
void sample_annulus(const double radius, const Point & center, uint & seed, Point & x)
{
    Point r;
    for(;;)
    {
        for(uint i=0; i<Dim; ++i)
        {
            r[i]=4*(random_uint(seed++)/static_cast<double>(max_uint)-0.5);
        }
        double r2=r.length_squared();
        if(r2>1 && r2<=4) break;
    }
    x=center+radius*r;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint Dim, class Point>
CINO_INLINE
void Poisson_sampling(const double          radius,
                      const Point           min,
                      const Point           max,
                      std::vector<Point> &  samples,
                      uint                  seed,
                      const int             max_attempts)
{
    samples.clear();
    std::vector<uint> active_list;

    // acceleration grid
    double step = 0.999*radius/std::sqrt(static_cast<double>(Dim)); // a grid cell this size can have at most one sample in it
    std::array<uint,Dim> dim_extent;
    unsigned long int grid_size = 1;
    for(uint i=0; i<Dim; ++i)
    {
        dim_extent[i] = static_cast<uint>(std::ceil((max[i]-min[i])/step));
        grid_size *= dim_extent[i];
    }
    std::vector<int> grid(grid_size, -1); // -1 indicates no sample there; otherwise index of sample point

    // first sample
    Point x;
    for(uint i=0; i<Dim; ++i)
    {
        x[i] = (max[i]-min[i])*(random_uint(seed++)/static_cast<double>(max_uint)) + min[i];
    }
    samples.push_back(x);
    active_list.push_back(0);
    uint index = serialize_nD_index<Dim,Point>(dim_extent, (x-min)/step);
    grid[index] = 0;

    while(!active_list.empty())
    {
        uint r = static_cast<int>(random_float(seed++, 0, active_list.size()-0.0001f));
        int  p = active_list[r];
        bool found_sample = false;
        Point j, jmin, jmax;

        for(int attempt=0; attempt<max_attempts; ++attempt)
        {
            sample_annulus<Dim,Point>(radius, samples[p], seed, x);

            // check this sample is within bounds
            for(uint i=0; i<Dim; ++i)
            {
                if(x[i]<min[i] || x[i]>max[i]) goto reject_sample;
            }

            // test proximity to nearby samples
            for(uint i=0; i<Dim; ++i)
            {
                int this_min = static_cast<int>((x[i]-radius-min[i])/step);

                if(this_min<0) this_min=0;
                else if(this_min>=(int)dim_extent[i]) this_min=dim_extent[i]-1;

                jmin[i]=(uint)this_min;
                int this_max=(int)((x[i]+radius-min[i])/step);

                if(this_max<0) this_max=0;
                else if(this_max>=(int)dim_extent[i]) this_max=dim_extent[i]-1;

                jmax[i]=(uint)this_max;
            }

            for(j=jmin;;)
            {
                // check if there's a sample at j that's too close to x
                index = serialize_nD_index<Dim,Point>(dim_extent, j);
                if(grid[index]>=0 && grid[index]!=p)
                {
                    // if there is a sample point different from p
                    if((x - samples[grid[index]]).length_squared()<radius*radius) goto reject_sample;
                }

                // move on to next j
                for(uint i=0; i<Dim; ++i)
                {
                    ++j[i];
                    if(j[i]<=jmax[i])
                    {
                        break;
                    }
                    else
                    {
                        if(i==Dim-1) goto done_j_loop;
                        else j[i]=jmin[i]; // and try incrementing the next dimension along
                    }
                }
            }

            done_j_loop:
            // if we made it here, we're good!
            found_sample=true;
            break;
            // if we goto here, x is too close to an existing sample
            reject_sample:
            ; // nothing to do except go to the next iteration in this loop
        }

        if(found_sample)
        {
            uint id=samples.size(); // the index of the new sample
            samples.push_back(x);
            active_list.push_back(id);
            index=serialize_nD_index<Dim,Point>(dim_extent, (x-min)/step);
            grid[index]=(int)id;
        }
        else
        {
            // since we couldn't find a sample on p's disk, we remove p from the active list
            active_list[r]=active_list.back();
            active_list.pop_back();
        }
    }
}

}
