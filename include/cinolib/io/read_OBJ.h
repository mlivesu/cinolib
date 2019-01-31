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
#ifndef CINO_READ_OBJ_H
#define CINO_READ_OBJ_H

#include <sys/types.h>
#include <map>
#include <string>
#include <vector>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/color.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_OBJ(const char                     * filename,
              std::vector<vec3d>             & pos,           // vertex xyz positions
              std::vector<vec3d>             & tex,           // vertex uv(w) texture coordinates
              std::vector<vec3d>             & nor,           // vertex normals
              std::vector<std::vector<uint>> & poly_pos,      // polygons with references to pos
              std::vector<std::vector<uint>> & poly_tex,      // polygons with references to tex
              std::vector<std::vector<uint>> & poly_nor,      // polygons with references to nor
              std::vector<Color>             & poly_col,      // per polygon colors
              std::string                    & diffuse_path,  // path of the image encoding the diffuse  texture component
              std::string                    & specular_path, // path of the image encoding the specular texture component
              std::string                    & normal_path);  // path of the image encoding the normal   texture component

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_OBJ(const char                     * filename,
              std::vector<vec3d>             & pos,         // vertex xyz positions
              std::vector<vec3d>             & tex,         // vertex uv(w) texture coordinates
              std::vector<vec3d>             & nor,         // vertex normals
              std::vector<std::vector<uint>> & poly_pos,    // polygons with references to pos
              std::vector<std::vector<uint>> & poly_tex,    // polygons with references to tex
              std::vector<std::vector<uint>> & poly_nor,    // polygons with references to nor
              std::vector<Color>             & poly_col);   // per polygon colors

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_OBJ(const char                     * filename,
              std::vector<vec3d>             & verts,
              std::vector<std::vector<uint>> & poly);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_OBJ(const char                     * filename,
              std::vector<vec3d>             & xyz,
              std::vector<vec3d>             & uvw,
              std::vector<std::vector<uint>> & poly);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_MTU(const char                  * filename,
              std::map<std::string,Color> & color_map,
              std::string                 & diffuse_path,  // path of the image encoding the diffuse  texture component
              std::string                 & specular_path, // path of the image encoding the specular texture component
              std::string                 & normal_path);  // path of the image encoding the normal   texture component

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_MTU(const char                  * filename,
              std::map<std::string,Color> & color_map);

}

#ifndef  CINO_STATIC_LIB
#include "read_OBJ.cpp"
#endif

#endif // CINO_READ_OBJ
