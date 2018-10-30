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
#include <cinolib/io/write_HEDRA.h>
#include <iostream>

namespace cinolib
{

CINO_INLINE
void write_HEDRA(const char                           * filename,
                 const std::vector<vec3d>             & verts,
                 const std::vector<std::vector<uint>> & faces,
                 const std::vector<std::vector<uint>> & polys,
                 const std::vector<std::vector<bool>> & polys_winding)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_HEDRA() : couldn't write output file " << filename << std::endl;
        exit(-1);
    }

    uint nv = verts.size();
    uint nf = faces.size();
    uint np = polys.size();

    fprintf(fp, "%d %d %d\n", nv, nf, np);

    // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
    //
    for(const vec3d & v : verts) fprintf(fp, "%.17g %.17g %.17g\n", v.x(), v.y(), v.z());

    for(const std::vector<uint> & f : faces)
    {
        fprintf(fp, "%d ", static_cast<int>(f.size()));
        for(uint vid : f) fprintf(fp, "%d ", vid+1);
        fprintf(fp, "\n");
    }

    for(uint pid=0; pid<np; ++pid)
    {
        fprintf(fp, "%d ", static_cast<int>(polys.at(pid).size()));

        for(uint off=0; off<polys.at(pid).size(); ++off)
        {
            if (polys_winding.at(pid).at(off)) fprintf(fp, "%d ",   polys.at(pid).at(off)+1);
            else                               fprintf(fp, "%d ", -(polys.at(pid).at(off)+1));
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

}
