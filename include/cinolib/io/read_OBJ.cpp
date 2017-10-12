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
#include <cinolib/io/read_OBJ.h>
#include <cinolib/cut_along_seams.h>
#include <string>
#include <sstream>
#include <iostream>
#include <assert.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// https://stackoverflow.com/questions/9310327/sscanf-optional-column
//
CINO_INLINE
void read_point_id(char * s, int & v, int & vt, int & vn)
{
    v = vt = vn = -1;

    if(sscanf(s, "%d/%d/%d", &v, &vt, &vn) == 3)
    {
        --v; --vt; --vn;
    }
    else if(sscanf(s, "%d/%d", &v, &vt) == 2)
    {
        --v; --vt;
    }
    else if(sscanf(s, "%d//%d", &v, &vn) == 2)
    {
        --v; --vn;
    }
    else if(sscanf(s, "%d", &v) == 1)
    {
        --v;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_OBJ(const char                     * filename,
              std::vector<vec3d>             & verts,
              std::vector<std::vector<uint>> & poly)
{
    std::vector<vec3d> tex, nor;
    std::vector<std::vector<uint>> poly_tex, poly_nor;
    read_OBJ(filename, verts, tex, nor, poly, poly_tex, poly_nor);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_OBJ(const char                     * filename,
              std::vector<vec3d>             & xyz,
              std::vector<vec3d>             & uvw,
              std::vector<std::vector<uint>> & poly)
{
    std::vector<vec3d> pos, tex, nor;
    std::vector<std::vector<uint>> poly_pos, poly_tex, poly_nor;
    read_OBJ(filename, pos, tex, nor, poly_pos, poly_tex, poly_nor);

    cut_mesh_along_seams(pos, tex, poly_pos, poly_tex, xyz, uvw, poly);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_OBJ(const char                     * filename,
              std::vector<vec3d>             & pos,
              std::vector<vec3d>             & tex,
              std::vector<vec3d>             & nor,
              std::vector<std::vector<uint>> & poly_pos,
              std::vector<std::vector<uint>> & poly_tex,
              std::vector<std::vector<uint>> & poly_nor)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_OBJ() : couldn't open input file " << filename << endl;
        exit(-1);
    }

    char line[1024];

    while(fgets(line, 1024, fp))
    {
        switch(line[0])
        {
            case 'v':
            {
                // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
                //
                double a, b, c;
                if(sscanf(line, "v %lf %lf %lf", &a, &b, &c) == 3)
                {
                    pos.push_back(vec3d(a,b,c));
                }
                else if(sscanf(line, "vt %lf %lf %lf", &a, &b, &c) == 3)
                {
                    tex.push_back(vec3d(a,b,c));
                }
                else if(sscanf(line, "vt %lf %lf %lf", &a, &b, &c) == 2)
                {
                    tex.push_back(vec3d(a,b,0));
                }
                else if(sscanf(line, "vn %lf %lf %lf", &a, &b, &c) == 3)
                {
                    nor.push_back(vec3d(a,b,c));
                }
                break;
            }

            case 'f':
            {
                std::string s(line);
                s = s.substr(1,s.size()-1); // discard the 'f' letter
                std::istringstream ss(s);
                std::vector<uint> p_pos, p_tex, p_nor;
                for(std::string sub_str; ss >> sub_str;)
                {
                    int v_pos, v_tex, v_nor;
                    read_point_id(strdup(sub_str.c_str()), v_pos, v_tex, v_nor);
                    if (v_pos >= 0) p_pos.push_back(v_pos);
                    if (v_tex >= 0) p_tex.push_back(v_tex);
                    if (v_nor >= 0) p_nor.push_back(v_nor);
                }
                poly_pos.push_back(p_pos);
                poly_tex.push_back(p_tex);
                poly_nor.push_back(p_nor);
                break;
            }
        }
    }
    fclose(fp);
}

}
