/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2024: Marco Livesu                                              *
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
#include <cinolib/stripe_embedding/flip_checks.h>
#include <cinolib/rationals.h>
#include <cinolib/predicates.h>

namespace cinolib
{

CINO_INLINE
bool flipped_d(SE_data & data,
               const uint v0,
               const uint v1,
               const uint v2)
{
    return (orient2d(&data.coords_d.at(2*v0),
                     &data.coords_d.at(2*v1),
                     &data.coords_d.at(2*v2)) <= 0);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool flipped_q(SE_data & data,
               const uint v0,
               const uint v1,
               const uint v2)
{
    return (orient2d(&data.coords_q.at(2*v0),
                     &data.coords_q.at(2*v1),
                     &data.coords_q.at(2*v2)) <= 0);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool flipped_m(SE_data & data,
               const uint v0,
               const uint v1,
               const uint v2)
{
    return (orient2d(&data.coords_m.at(2*v0),
                     &data.coords_m.at(2*v1),
                     &data.coords_m.at(2*v2)) <= 0);
}

}
