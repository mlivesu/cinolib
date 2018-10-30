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
#include <cinolib/io/write_MESH.h>

#include <iostream>

namespace cinolib
{

CINO_INLINE
void write_MESH(const char                           * filename,
                const std::vector<vec3d>             & verts,
                const std::vector<std::vector<uint>> & polys,
                const std::vector<int>               & vert_labels,
                const std::vector<int>               & poly_labels)
{
    assert(vert_labels.size() == verts.size());
    assert(poly_labels.size() == polys.size());

    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_MESH() : couldn't write output file " << filename << std::endl;
        exit(-1);
    }

    fprintf(fp, "MeshVersionFormatted 1\n" );
    fprintf(fp, "Dimension 3\n" );

    uint nv = verts.size();
    uint nt = 0;
    uint nh = 0;
    for(auto p : polys)
    {
        if (p.size() == 4) ++nt; else
        if (p.size() == 8) ++nh;
    }

    if (nv > 0)
    {
        fprintf(fp, "Vertices\n" );
        fprintf(fp, "%d\n", nv);
        for(uint vid=0; vid<nv; ++vid)
        {
            // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
            //
            fprintf( fp, "%.17g %.17g %.17g %d\n", verts.at(vid).x(), verts.at(vid).y(), verts.at(vid).z(), vert_labels.at(vid));
        }
    }

    if (nt > 0)
    {
        fprintf(fp, "Tetrahedra\n" );
        fprintf(fp, "%d\n", nt );
        for(uint pid=0; pid<polys.size(); ++pid)
        {
            const std::vector<uint> & tet = polys.at(pid);
            if (tet.size() == 4)
            {
                fprintf(fp, "%d %d %d %d %d\n", tet.at(0)+1, tet.at(1)+1, tet.at(2)+1, tet.at(3)+1, poly_labels.at(pid));
            }
        }
    }

    if (nh > 0)
    {
        fprintf(fp, "Hexahedra\n" );
        fprintf(fp, "%d\n", nh );
        for(uint pid=0; pid<polys.size(); ++pid)
        {
            const std::vector<uint> & hex = polys.at(pid);
            if (hex.size() == 8)
            {
                fprintf(fp, "%d %d %d %d %d %d %d %d %d\n", hex.at(0)+1, hex.at(1)+1, hex.at(2)+1, hex.at(3)+1,
                                                            hex.at(4)+1, hex.at(5)+1, hex.at(6)+1, hex.at(7)+1,
                                                            poly_labels.at(pid));
            }
        }
    }

    fprintf(fp, "End\n\n");
    fclose(fp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void write_MESH(const char                           * filename,
                const std::vector<vec3d>             & verts,
                const std::vector<std::vector<uint>> & polys)
{
    std::vector<int> vert_labels(verts.size(),0);
    std::vector<int> poly_labels(polys.size(),0);
    write_MESH(filename, verts, polys, vert_labels, poly_labels);
}

}
