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
#include <cinolib/io/read_OFF.h>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_OFF(const char                     * filename,
              std::vector<vec3d>             & verts,
              std::vector<std::vector<uint>> & polys,
              std::vector<Color>             & poly_colors)
{
    verts.clear();
    polys.clear();
    poly_colors.clear();

    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    std::ifstream f(filename);
    if(!f.is_open())
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_OFF() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    std::string line;
    uint        nv, np, ne;

    // read header and number of elements
    do getline(f, line, '\n'); while(line.find("OFF")==std::string::npos);
    do getline(f, line, '\n'); while(sscanf(line.c_str(), "%d %d %d\n", &nv, &np, &ne)!=3);

    // read verts
    for(uint i=0; i<nv; ++i)
    {
        getline(f, line, '\n');
        std::stringstream ss(line);

        double x, y, z;
        if(ss >> x >> y >> z)
        {
            verts.push_back(vec3d(x,y,z));
        }
        else --i;
    }

    // read polys
    for(uint i=0; i<np; ++i)
    {
        getline(f, line, '\n');
        std::stringstream ss(line);

        uint n_corners;
        if(ss >> n_corners)
        {
            uint vid;
            std::vector<uint> p;
            for(uint j=0; j<n_corners; ++j)
            {
                assert(ss >> vid);
                p.push_back(vid);
            }
            polys.push_back(p);

            float val;
            std::vector<float> attr;
            while(ss >> val) attr.push_back(val);

            switch(attr.size())
            {
                case 1 : break; // TODO: READ LABEL (cast to int)!!!
                case 3 : poly_colors.push_back(Color(attr.at(0), attr.at(1), attr.at(2))); break;
                case 4 : poly_colors.push_back(Color(attr.at(0), attr.at(1), attr.at(2), attr.at(3))); break;
                default: break;
            }
        }
        else --i;
    }
}

}
