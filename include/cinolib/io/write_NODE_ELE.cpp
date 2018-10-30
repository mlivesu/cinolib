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
#include <cinolib/io/write_NODE_ELE.h>
#include <iostream>

namespace cinolib
{

CINO_INLINE
void write_NODE_ELE(const char                           * basename,
                    const std::vector<vec3d>             & verts,
                    const std::vector<std::vector<uint>> & poly)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    std::string node_filename = std::string(basename) + ".node";
    std::string ele_filename  = std::string(basename) + ".ele";

    FILE *f_node = fopen(node_filename.c_str(), "w");
    FILE *f_ele  = fopen(ele_filename.c_str(),  "w");

    if(!f_node || !f_ele)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write_NODE_ELE() : couldn't create output file(s)." << std::endl;
        exit(-1);
    }

    fprintf(f_node, "%d 0\n", (int)verts.size());
    for(auto p : verts)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fprintf(f_node, "%.17g %.17g %.17g\n", p.x(), p.y(), p.z());
    }

    fprintf(f_ele, "%d\n", (int)poly.size());
    for(const std::vector<uint> & p : poly)
    {
        fprintf(f_ele, "%d ", (int)p.size());
        for(uint vid : p) fprintf(f_ele, "%d ", vid+1);
        fprintf(f_ele, "\n");
    }

    fclose(f_node);
    fclose(f_ele);
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void write_NODE_ELE_2D(const char                           * basename,
                       const std::vector<vec3d>             & verts,
                       const std::vector<std::vector<uint>> & poly)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    std::string node_filename = std::string(basename) + ".node";
    std::string ele_filename  = std::string(basename) + ".ele";

    FILE *f_node = fopen(node_filename.c_str(), "w");
    FILE *f_ele  = fopen(ele_filename.c_str(),  "w");

    if(!f_node || !f_ele)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write_NODE_ELE() : couldn't create output file(s)." << std::endl;
        exit(-1);
    }

    fprintf(f_node, "%d 0\n", (int)verts.size());
    for(auto p : verts)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fprintf(f_node, "%.17g %.17g\n", p.x(), p.y());
    }

    fprintf(f_ele, "%d\n", (int)poly.size());
    for(const std::vector<uint> & p : poly)
    {
        fprintf(f_ele, "%d ", (int)p.size());
        for(uint vid : p) fprintf(f_ele, "%d ", vid+1);
        fprintf(f_ele, "\n");
    }

    fclose(f_node);
    fclose(f_ele);
}

}
