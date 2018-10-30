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
#ifndef CINO_TETGEN_WRAP_H
#define CINO_TETGEN_WRAP_H

#include <vector>
#include <sys/types.h>
#include <string>
#include <cinolib/cino_inline.h>
#include <cinolib/meshes/tetmesh.h>


namespace cinolib
{

CINO_INLINE
void tetgen_wrap(const std::vector<double>            & coords_in,
                 const std::vector<std::vector<uint>> & polys_in,
                 const std::vector<uint>              & edges_in,
                 const std::string                    & flags,       // options
                       std::vector<double>            & coords_out,
                       std::vector<uint>              & tets_out);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void tetgen_wrap(const std::vector<double> & coords_in,
                 const std::vector<uint>   & tris_in,
                 const std::vector<uint>   & edges_in,
                 const std::string         & flags,       // options
                       std::vector<double> & coords_out,
                       std::vector<uint>   & tets_out);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void tetgen_wrap(const std::vector<vec3d>             & verts_in,
                 const std::vector<std::vector<uint>> & polys_in,
                 const std::vector<uint>              & edges_in,
                 const std::string                    & flags,       // options
                       std::vector<vec3d>             & coords_out,
                       std::vector<uint>              & tets_out);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void tetgen_wrap(const std::vector<vec3d>  & verts_in,
                 const std::vector<uint>   & tris_in,
                 const std::vector<uint>   & edges_in,
                 const std::string         & flags,       // options
                       std::vector<vec3d>  & coords_out,
                       std::vector<uint>   & tets_out);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void tetgen_wrap(const std::vector<vec3d>  & verts_in,
                 const std::vector<uint>   & tris_in,
                 const std::vector<uint>   & edges_in,
                 const std::string         & flags,       // options
                       Tetmesh<M,V,E,F,P>  & m);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void tetgen_wrap(const std::vector<vec3d>             & verts_in,
                 const std::vector<std::vector<uint>> & polys_in,
                 const std::vector<uint>              & edges_in,
                 const std::string                    & flags,       // options
                       Tetmesh<M,V,E,F,P>             & m);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void tetgen_wrap(const AbstractPolygonMesh<M,V,E,F> & m_srf,
                 const std::string                  & flags,       // options
                       Tetmesh<M,V,E,F,P>           & m);
}

#ifndef  CINO_STATIC_LIB
#include "tetgen_wrap.cpp"
#endif

#endif // CINO_TETGEN_WRAP_H
