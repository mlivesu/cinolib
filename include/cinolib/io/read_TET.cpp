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
#include <cinolib/io/read_TET.h>
#include <iostream>

namespace cinolib
{

CINO_INLINE
void read_TET(const char          * filename,
              std::vector<double> & xyz,
              std::vector<uint>  & tets)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_TET() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    uint  nv, nt;
    char line[1024];

    fgets(line,1024,fp);
    sscanf(line, "%d vertices", &nv);

    fgets(line,1024,fp);
    sscanf(line, "%d tets", &nt);

    for(uint vid=0; vid<nv; ++vid)
    {
        fgets(line, 1024, fp);

        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        double x,y,z;
        sscanf(line, "%lf %lf %lf ", &x, &y, &z);

        xyz.push_back(x);
        xyz.push_back(y);
        xyz.push_back(z);
    }

    for(uint tid=0; tid<nt; ++tid)
    {
        fgets(line, 1024, fp);

        uint v0, v1, v2, v3;
        sscanf(line, "4 %d %d %d %d", &v0, &v1, &v2, &v3);

        tets.push_back(v0);
        tets.push_back(v3);
        tets.push_back(v2);
        tets.push_back(v1);
    }

    fclose(fp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_TET(const char                     * filename,
              std::vector<vec3d>             & verts,
              std::vector<std::vector<uint>> & polys)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_TET() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    uint  nv, np;
    char line[1024];

    fgets(line,1024,fp);
    sscanf(line, "%d vertices", &nv);

    fgets(line,1024,fp);
    sscanf(line, "%d tets", &np);

    verts.reserve(nv);
    polys.reserve(np);

    for(uint vid=0; vid<nv; ++vid)
    {
        fgets(line, 1024, fp);

        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        vec3d p;
        sscanf(line, "%lf %lf %lf ", &p.x(), &p.y(), &p.z());
        verts.push_back(p);
    }

    for(uint pid=0; pid<np; ++pid)
    {
        fgets(line, 1024, fp);

        uint v0, v1, v2, v3;
        sscanf(line, "4 %d %d %d %d", &v0, &v1, &v2, &v3);

        std::vector<uint> tet;
        tet.push_back(v0);
        tet.push_back(v3);
        tet.push_back(v2);
        tet.push_back(v1);
        polys.push_back(tet);
    }

    fclose(fp);
}

}
