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
#include <cinolib/io/read_HEDRA.h>
#include <iostream>

namespace cinolib
{

CINO_INLINE
void read_HEDRA(const char                     * filename,
                std::vector<vec3d>             & verts,
                std::vector<std::vector<uint>> & faces,
                std::vector<std::vector<uint>> & polys,
                std::vector<std::vector<bool>> & polys_winding)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    verts.clear();
    faces.clear();
    polys.clear();
    polys_winding.clear();

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_HEDRA() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    uint nv, nf, np;
    fscanf(fp, "%d %d %d", &nv, &nf, &np);

    for(uint i=0; i<nv; ++i)
    {
        double x, y, z;
        fscanf(fp, "%lf %lf %lf", &x, &y, &z);
        verts.push_back(vec3d(x,y,z));
    }

    for(uint i=0; i<nf; ++i)
    {
        uint n_verts;
        fscanf(fp, "%d", &n_verts);

        std::vector<uint> f;
        for(uint j=0; j<n_verts; ++j)
        {
            uint vid;
            fscanf(fp, "%d", &vid);
            f.push_back(vid-1);
        }
        faces.push_back(f);
    }

    for(uint i=0; i<np; ++i)
    {
        uint nf;
        fscanf(fp, "%d", &nf);

        std::vector<uint> p;
        std::vector<bool> p_winding;
        for(uint j=0; j<nf; ++j)
        {
            int fid;
            fscanf(fp, "%d", &fid);

            if (fid > 0)
            {
                p.push_back(static_cast<uint>(fid-1));
                p_winding.push_back(true);
            }
            else
            {
                p.push_back(static_cast<uint>(std::fabs(fid)-1));
                p_winding.push_back(false);
            }
        }
        polys.push_back(p);
        polys_winding.push_back(p_winding);
    }

    fclose(fp);
}

}
