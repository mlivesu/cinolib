/*********************************************************************************
*  Copyright(C) 2016: Marco Livesu                                               *
*  All rights reserved.                                                          *
*                                                                                *
*  This file is part of CinoLib                                                  *
*                                                                                *
*  CinoLib is dual-licensed:                                                     *
*                                                                                *
*   - For non-commercial use you can redistribute it and/or modify it under the  *
*     terms of the GNU General Public License as published by the Free Software  *
*     Foundation; either version 3 of the License, or (at your option) any later *
*     version.                                                                   *
*                                                                                *
*   - If you wish to use it as part of a commercial software, a proper agreement *
*     with the Author(s) must be reached, based on a proper licensing contract.  *
*                                                                                *
*  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE       *
*  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.     *
*                                                                                *
*  Author(s):                                                                    *
*                                                                                *
*     Marco Livesu (marco.livesu@gmail.com)                                      *
*     http://pers.ge.imati.cnr.it/livesu/                                        *
*                                                                                *
*     Italian National Research Council (CNR)                                    *
*     Institute for Applied Mathematics and Information Technologies (IMATI)     *
*     Via de Marini, 6                                                           *
*     16149 Genoa,                                                               *
*     Italy                                                                      *
**********************************************************************************/
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
