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
#include <cinolib/AFM/snap_rounding.h>
#include <cinolib/AFM/flip_checks.h>

namespace cinolib
{

CINO_INLINE
bool snap_rounding(AFM_data & data, const uint vid)
{
    if(!data.enable_snap_rounding) return true;

    // keep a safe copy of the exact coordinates
    CGAL_Q tmp[3];
    copy(&data.exact_coords[3*vid],tmp);

    // round them to the closest double
    data.exact_coords[3*vid+0] = CGAL::to_double(tmp[0]);
    data.exact_coords[3*vid+1] = CGAL::to_double(tmp[1]);
    data.exact_coords[3*vid+2] = CGAL::to_double(tmp[2]);

    // check for flips
    bool flips = false;
    for(uint pid : data.m1.adj_v2p(vid))
    {
        if(flipped(data,pid))
        {
            flips = true;
            break;
        }
    }

    if(flips) // rollback
    {
        copy(tmp, &data.exact_coords[3*vid]);
        ++data.snap_roundings_failed;
        return false;
    }
    return true;
}

}
