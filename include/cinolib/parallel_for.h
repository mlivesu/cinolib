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
#ifndef CINO_PARALLEL_FOR_H
#define CINO_PARALLEL_FOR_H

#include <sys/types.h>
#include <cinolib/cino_inline.h>

namespace cinolib
{

/* OpenMP-like parallel for loop realized in plain C++11
 * Thanks to Jeremy Dumas for his code (https://ideone.com/Z7zldb)
 *
 * DISCLAIMER: this routine is as good as OpenMP with STATIC scheduling.
 * That is, it works best when computational cost is equally distributed
 * across all threads. In case of unbalanced distribution, performances
 * may degrade, and using the DYNAMIC scheduling of a library such as OpenMP
 * will greatly benefit performances.
 *
 * PARALLEL_FOR has three arguments
 *
 *     beg,end             : define a range of indices
 *     serial_if_less_than : avoid paying the overhead if the range is smaller than...
 *     func                : is the function that implements the body of the loop.
 *                           It takes as unique argument the loop index. This will
 *                           typically be a lambda function inlined in the call
 *
 * Example of usage: update normals on a mesh.
 * Given a polygonmesh m, the classical serial loop would be like:
 *
 * for(int pid=0; i<m.num_polys(); ++pid)
 * {
 *     m.update_p_normal(pid);
 * }
 *
 * Normal computation is indeed local to each polygon,
 * therefore this code can be parallelized with the call:
 *
 * PARALLEL_FOR(0, m.num_polys(), 1000, [&m](int pid)
 * {
 *    m.update_p_normal(pid);
 * });
 *
 * NOTE: if symbol SERIALIZE_PARALLEL_FOR is defined at compilation time,
 * the loop will be executed in standard serial mode.
*/

template<typename Func>
CINO_INLINE
static void PARALLEL_FOR(      uint   beg,
                               uint   end,
                         const uint   serial_if_less_than,
                         const Func & func);
}

#ifndef  CINO_STATIC_LIB
#include "parallel_for.cpp"
#endif

#endif // CINO_PARALLEL_FOR_H
