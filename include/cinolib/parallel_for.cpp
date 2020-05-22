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
#include <cinolib/parallel_for.h>
#include <thread>
#include <vector>
#include <cmath>

namespace cinolib
{

template<typename Func>
CINO_INLINE
static void PARALLEL_FOR(      uint   beg,
                               uint   end,
                         const uint   serial_if_less_than,
                         const Func & func)
{
#ifndef SERIALIZE_PARALLEL_FOR

    uint n = end - beg + 1;

    if(n<serial_if_less_than)
    {
        for(uint i=beg; i<end; ++i) func(i);
    }
    else
    {
        // estimate number of threads in the pool
        const static unsigned n_threads_hint = std::thread::hardware_concurrency();
        const static unsigned n_threads      = (n_threads_hint==0u) ? 8u : n_threads_hint;

        // split the full range into sub ranges of equal size
        uint slice = (uint)std::round(n/static_cast<double>(n_threads));
        slice = std::max(slice,uint(1));

        // helper function that handles a sub range
        auto subrange_helper = [&func](uint k1, uint k2)
        {
            for(uint k=k1; k<k2; ++k) func(k);
        };

        // create pool and launch jobs
        std::vector<std::thread> pool;
        pool.reserve(n_threads);
        uint i1 = beg;
        uint i2 = std::min(beg + slice, end);
        for(uint i=0; i+1<n_threads && i1<end; ++i)
        {
            pool.emplace_back(subrange_helper, i1, i2);
            i1 = i2;
            i2 = std::min(i2+slice, end);
        }
        if(i1<end) pool.emplace_back(subrange_helper, i1, end);

        // Wait for jobs to finish
        for(std::thread & t : pool)
        {
            if(t.joinable()) t.join();
        }
    }
#else
    for(uint i=beg; i<end; ++i) func(i);
#endif
}

}
