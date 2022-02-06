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
#ifndef CINO_SUPPORTS_VOLUME_H
#define CINO_SUPPORTS_VOLUME_H

#include <cinolib/meshes/trimesh.h>

namespace cinolib
{

// computes the total volume of the support structures necessary to sustain
// the overhangs of mesh m if printed along the given build direction.
// Supports are assumed to emanate from each overhang and propagate downwards
// until they hit the building platform (floor) or some mesh triangle.
//
// The supporting volume is modelled as the volume of a thick prism with
// triangular cross section. More precisely, for each haning triangle t,
// the estimated volume is:
//
//    Vol[Support(t)] = area(t) * height
//
// where heght is either the distance of the centroid of t from the floor,
// or its distance from a triangle positioned below t along the build direction.
//
// WARNING: this function provides just a rough approximation of the supports'
// volume, mainly for two reasons:
//
//     1) the contact points between supports and object are not modelled,
//        therefore tiny volume variations may arise
//
//     2) in reality supports will not be a dense extrusion of overhangs,
//        but will likely have a much sparser structure
//
// For (1) we assume that approximation errors are negligible. For (2)
// the users may want to scale the output volume by a density factor that
// takes into account the sparsity of the real supports. The necessity to
// perform this operation depends on the specific application. E.g., if one
// want to find the build direction that minimizes supports' volume, using
// a density factor is unnecessary. If one wants to estimate the actual
// material used for printing such a scaling is necessary, otherwise the
// amunt of material will be severely overestimated.
//
template<class M, class V, class E, class P>
CINO_INLINE
float supports_volume(const Trimesh<M,V,E,P>                  & m,
                      const vec3d                             & build_dir,
                      const float                               floor,
                      const std::vector<std::pair<uint,uint>> & overhangs);

}

#ifndef  CINO_STATIC_LIB
#include "supports_volume.cpp"
#endif

#endif // CINO_SUPPORTS_VOLUME_H
