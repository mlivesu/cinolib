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
#include <cinolib/io/read_MESH.h>
#include <cinolib/vector_serialization.h>
#include <iostream>
#include <set>

namespace cinolib
{

CINO_INLINE
void read_MESH(const char                     * filename,
               std::vector<vec3d>             & verts,
               std::vector<std::vector<uint>> & polys,
               std::vector<int>               & vert_labels,
               std::vector<int>               & poly_labels)
{
    verts.clear();
    polys.clear();
    vert_labels.clear();
    poly_labels.clear();

    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *f = fopen(filename, "r");
    if(!f)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_MESH() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    char line[1024];
    fgets(line,1024,f);
    fgets(line,1024,f);

    std::set<int> v_unique_labels;
    std::set<int> p_unique_labels;

    while(fgets(line, 1024, f))
    {
        // read vertices
        //
        if (line[0] == 'V' || (line[0] == ' ' && line[1] == 'V' ))
        {
            uint   nverts;
            double x,y,z;
            int    l;
            fgets(line,1024,f);
            sscanf(line, "%d", &nverts);
            for(uint i=0; i<nverts; ++i)
            {
                // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
                //
                fgets(line, 1024, f);
                sscanf(line, "%lf %lf %lf %d", &x, &y, &z, &l);
                verts.push_back(vec3d(x,y,z));
                vert_labels.push_back(l);
                v_unique_labels.insert(l);
            }
        }

        // read tetrahedra
        //
        if ((line[0] == 'T' && line[1] == 'e') || ( line[0] == ' ' && line[1] == 'T' && line[2] == 'e')  )
        {
            uint ntets;
            fgets(line, 1024, f);
            sscanf(line, "%d", &ntets);
            for(uint i=0; i<ntets; ++i)
            {
                int l;
                std::vector<uint> tet(4);
                fgets(line,1024,f);
                sscanf(line, "%d %d %d %d %d", &tet[0], &tet[1], &tet[2], &tet[3], &l);
                for(uint & vid : tet) vid -= 1;
                polys.push_back(tet);
                poly_labels.push_back(l);
                p_unique_labels.insert(l);
            }
        }

        // read hexahedra
        //
        if (line[0] == 'H' && line[1] == 'e')
        {
            uint nhexa;
            fgets(line, 1024, f);
            sscanf(line, "%d", &nhexa);
            for(uint i=0; i<nhexa; ++i)
            {
                int l;
                std::vector<uint> hex(8);
                fgets(line,1024,f);
                sscanf(line, "%d %d %d %d %d %d %d %d %d", &hex[0], &hex[1], &hex[2], &hex[3],
                                                           &hex[4], &hex[5], &hex[6], &hex[7], &l);
                for(uint & vid : hex) vid -= 1;
                polys.push_back(hex);
                poly_labels.push_back(l);
                p_unique_labels.insert(l);
            }
        }
    }

    if(v_unique_labels.size()<2) vert_labels.clear();
    if(p_unique_labels.size()<2) poly_labels.clear();

    fclose(f);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_MESH(const char                     * filename,
               std::vector<vec3d>             & verts,
               std::vector<std::vector<uint>> & polys)
{
    std::vector<int> vert_labels, poly_labels;
    read_MESH(filename, verts, polys, vert_labels, poly_labels);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_MESH(const char                     * filename,
               std::vector<double>            & coords,
               std::vector<std::vector<uint>> & polys)
{
    std::vector<vec3d> verts;
    std::vector<int>   vert_labels, poly_labels;
    read_MESH(filename, verts, polys, vert_labels, poly_labels);
    coords = serialized_xyz_from_vec3d(verts);
}

}
