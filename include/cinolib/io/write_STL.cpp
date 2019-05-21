/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2019: Daniela Cabiddu
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
*     Daniela Cabiddu (daniela.cabiddu@ge.imati.cnr.it)                         *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#include <cinolib/io/write_STL.h>
#include <iostream>

namespace cinolib
{

CINO_INLINE
void write_STL(const char                           * filename,
               const std::vector<double>            & xyz,
               const std::vector<std::vector<uint>> & poly,
               const std::vector<double>            & normals)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");
    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_STL() : couldn't save file " << filename << std::endl;
        exit(-1);
    }

    fprintf(fp, "solid cinolib_mesh\n");
    for(uint pid=0; pid<poly.size(); ++pid)
    {
        fprintf(fp, "facet normal %f %f %f\n", normals.at(pid*3+0), normals.at(pid*3+1), normals.at(pid*3+2));
        fprintf(fp, "  outer loop\n");
        fprintf(fp, "    vertex %f %f %f\n", xyz.at(poly.at(pid).at(0)*3+0), xyz.at(poly.at(pid).at(0)*3+1), xyz.at(poly.at(pid).at(0)*3+2));
        fprintf(fp, "    vertex %f %f %f\n", xyz.at(poly.at(pid).at(1)*3+0), xyz.at(poly.at(pid).at(1)*3+1), xyz.at(poly.at(pid).at(1)*3+2));
        fprintf(fp, "    vertex %f %f %f\n", xyz.at(poly.at(pid).at(2)*3+0), xyz.at(poly.at(pid).at(2)*3+1), xyz.at(poly.at(pid).at(2)*3+2));
        fprintf(fp, "  endloop\n");
        fprintf(fp, "endfacet\n");
    }
    fprintf(fp, "endsolid cinolib_mesh\n");
    fclose(fp);
}

}
