/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2023: Marco Livesu                                              *
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
#include <cinolib/AFM/flip_checks.h>
#include <cinolib/rationals.h>
#include <cinolib/predicates.h>

namespace cinolib
{

CINO_INLINE
bool flipped(AFM_data & data,
             const uint a,
             const uint b,
             const uint c)
{
    return orient2d(&data.exact_coords[3*a],
                    &data.exact_coords[3*b],
                    &data.exact_coords[3*c]) <= 0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool flipped(AFM_data & data, const uint pid, const bool use_rationals)
{
    if(use_rationals)
    {
        return orient2d(&data.exact_coords[3*data.m1.poly_vert_id(pid,0)],
                        &data.exact_coords[3*data.m1.poly_vert_id(pid,1)],
                        &data.exact_coords[3*data.m1.poly_vert_id(pid,2)]) <= 0;
    }
    return orient2d(data.m1.poly_vert(pid,0).ptr(),
                    data.m1.poly_vert(pid,1).ptr(),
                    data.m1.poly_vert(pid,2).ptr()) <= 0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint count_flipped(AFM_data & data, const bool use_rationals)
{
    uint count = 0;
    for(uint pid=0; pid<data.m1.num_polys(); ++pid)
    {
        if(flipped(data,pid,use_rationals)) ++count;
    }
    return count;
}

}
