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
#include <string>
#include <sstream>
#include <iostream>

namespace cinolib
{

CINO_INLINE
uint read_point_id(char *s)
{
    int pos = 0;
    while(s[pos] != '\0')
    {
        if(s[pos] == '/')
        {
            s[pos] = ' ';
        }
        ++pos;
    }
    sscanf(s, "%d", &pos);
    return pos-1;
}


CINO_INLINE
void read_OBJ(const char           * filename,
              std::vector<double>  & xyz,
              std::vector<u_int>   & tri,
              std::vector<u_int>   & quad)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_OBJ() : couldn't open input file " << filename << endl;
        exit(-1);
    }

    char line[1024], s0[1024], s1[1024], s2[1024], s3[1024];

    while(fgets(line, 1024, fp))
    {
        switch(line[0])
        {
            case 'v':

                if(!isspace(line[1])) continue;
                if(line[1] != 't' && line[1] != 'n' )
                {
                    // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
                    //
                    double x, y, z;
                    sscanf( line, "%*c %lf %lf %lf", &x, &y, &z );
                    xyz.push_back(x);
                    xyz.push_back(y);
                    xyz.push_back(z);
                }
                break;

            case 'f':
                int n_corners = sscanf(line, "%*c %s %s %s %s", s0, s1, s2, s3);
                if (n_corners == 3)
                {
                    tri.push_back(read_point_id(s0));
                    tri.push_back(read_point_id(s1));
                    tri.push_back(read_point_id(s2));
                }
                else if (n_corners == 4)
                {
                    quad.push_back(read_point_id(s0));
                    quad.push_back(read_point_id(s1));
                    quad.push_back(read_point_id(s2));
                    quad.push_back(read_point_id(s3));
                }
                else
                {
                    std::cerr << "read_OBJ: polygons with " << n_corners << " corners are not supported!" << std::endl;
                    assert("Unsupported polygon" && false);
                }
                break;
        }
    }
    fclose(fp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_OBJ(const char                     * filename,
              std::vector<double>            & xyz,
              std::vector<std::vector<uint>> & faces)
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

                if(!isspace(line[1])) continue;
                if(line[1] != 't' && line[1] != 'n' )
                {
                    // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
                    //
                    double x, y, z;
                    sscanf( line, "%*c %lf %lf %lf", &x, &y, &z );
                    xyz.push_back(x);
                    xyz.push_back(y);
                    xyz.push_back(z);
                }
                break;

            case 'f':
                std::string s(line);
                s = s.substr(1,s.size()-1); // discard the 'f' letter
                std::istringstream ss(s);
                std::vector<uint> face;
                for(uint vid; ss >> vid;) face.push_back(vid-1);
                faces.push_back(face);
                break;
        }
    }
    fclose(fp);
}

}
