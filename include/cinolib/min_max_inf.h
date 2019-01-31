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
#ifndef CINO_MIN_MAX_INF
#define CINO_MIN_MAX_INF

#include <sys/types.h>
#include <limits>

namespace cinolib
{
    // https://stackoverflow.com/questions/20016600/negative-infinity
    static_assert(std::numeric_limits<double>::is_iec559, "IEEE 754 required");

    const double min_double = std::numeric_limits<double>::min();
    const float  min_float  = std::numeric_limits<float>::min();
    const int    min_int    = std::numeric_limits<int>::min();
    const uint   min_uint   = std::numeric_limits<uint>::min();

    const double max_double = std::numeric_limits<double>::max();
    const float  max_float  = std::numeric_limits<float>::max();
    const int    max_int    = std::numeric_limits<int>::max();
    const uint   max_uint   = std::numeric_limits<uint>::max();

    const double inf_double = std::numeric_limits<double>::infinity();
    const float  inf_float  = std::numeric_limits<float>::infinity();
    const int    inf_int    = std::numeric_limits<int>::infinity();
    const uint   inf_uint   = std::numeric_limits<uint>::infinity();
}

#endif // CINO_MIN_MAX_INF
