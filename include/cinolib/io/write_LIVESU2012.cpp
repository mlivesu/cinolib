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
#include <cinolib/io/write_LIVESU2012.h>

#include <vector>

namespace cinolib
{

CINO_INLINE
void write_Livesu2012(const char                       * filename,
                      const std::vector<double>           coords,
                      const std::vector<double>           max_spheres,
                      const std::vector<std::vector<int>> adj_vtx2vtx)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *f = fopen(filename,"w");

    if (!f)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_Livesu2012() : error while opening file " << filename << std::endl;
        exit(-1);
    }

    uint nv = adj_vtx2vtx.size();

    fprintf( f, "ID Cx Cy Cz RADIUS #NEIGHBORS NEIGHBORS_LIST\n%d\n", nv);

    for(uint vid=0; vid<nv; ++vid)
    {
        vec3d pos(coords[3*vid+0],
                  coords[3*vid+1],
                  coords[3*vid+2]);

        std::vector<int> nbrs = adj_vtx2vtx.at(vid);

        fprintf(f,
                "%d %f %f %f %f %d ",
                vid,
                pos.x(),
                pos.y(),
                pos.z(),
                max_spheres.at(vid),
                (int)nbrs.size());

        for(uint i=0; i<nbrs.size(); ++i)
        {
            fprintf(f, "%d ", nbrs[i]);
        }

        fprintf(f, "\n");
    }

    fclose(f);
}

}
