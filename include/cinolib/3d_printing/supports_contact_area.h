/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2022: Marco Livesu                                              *
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
#ifndef CINO_SUPPORTS_CONTACT_AREA_H
#define CINO_SUPPORTS_CONTACT_AREA_H

#include <cinolib/meshes/trimesh.h>

namespace cinolib
{

// computes the total area of the surface patches where support
// structures are attached. Both overhanging triangles and triangles
// that lie below an overhang along the build direction are accounted
// for. The information about overhangs is assumed in input and can
// be computed with the function cinolib::overhangs. Each overhang is
// represented as a pair of triangle ids. The first one is the ID of
// the hanging triangle, the second one is the ID of a triangle lying
// below it (if any). In case the overhanging triangle projects directly
// onto the building platform, the second element of the pair is the
// ID of the hanging triangle itself
//
// WARNING: an overhanging triangle may project over multiple triangles,
// entirely or partially. Since the input data allows to specify the ID
// of a single triangle, the computation of the contact area is approximated.
// Nevertheless, assuming an even tessellation, the accuracy of the result
// should be good enough for most applications that necessitate this type
// of information (e.g. for optimally orient an object prior printing)
//
template<class M, class V, class E, class P>
CINO_INLINE
float supports_contact_area(const Trimesh<M,V,E,P>                  & m,
                            const std::vector<std::pair<uint,uint>> & overhangs);

}

#ifndef  CINO_STATIC_LIB
#include "supports_contact_area.cpp"
#endif

#endif // CINO_SUPPORTS_CONTACT_AREA_H
