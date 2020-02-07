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
#include <cinolib/io/io_utilities.h>
#include <iostream>
#include <unordered_set>

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

    std::unordered_set<int> v_unique_labels;
    std::unordered_set<int> p_unique_labels;

    // read header
    int ver, dim, nv, nc;
    if(!seek_keyword(f, "MeshVersionFormatted")) assert(false && "could not find keyword MESHVERSIONFORMATTED");
    eat_int(f, ver);
    if(!seek_keyword(f, "Dimension")) assert(false && "could not find keyword DIMENSION");
    eat_int(f, dim);

    // read verts
    if(!seek_keyword(f, "Vertices")) assert(false && "could not find keyword VERTICES");
    eat_int(f, nv);
    for(int i=0; i<nv; ++i)
    {
        double x=0,y=0,z=0;
        int l;
        if(!eat_double(f, x) ||
           !eat_double(f, y) ||
           !eat_double(f, z) ||
           !eat_int(f, l)) assert(false && "failed reading vert");
        verts.push_back(vec3d(x,y,z));
        vert_labels.push_back(l);
        v_unique_labels.insert(l);
    }

    // read cells
    char cell_type[50];
    while(eat_word(f, cell_type))
    {
        if(strcmp(cell_type, "End")==0)
        {
            fclose(f);
            if(v_unique_labels.size()<2) vert_labels.clear();
            if(p_unique_labels.size()<2) poly_labels.clear();
            return;
        }
        else if(strcmp(cell_type, "#")==0)
        {
            // comment, ignore whole line up to next \n
            char line[1024];
            char *unused;
            unused = fgets(line, 1024, f);
        }
        else if(strcmp(cell_type, "Tetrahedra")==0)
        {
            if(!eat_int(f, nc)) assert(false && "failed reading num tets");
            for(int i=0; i<nc; ++i)
            {
                int l;
                std::vector<uint> tet(4);
                if(!eat_uint(f, tet[0]) ||
                   !eat_uint(f, tet[1]) ||
                   !eat_uint(f, tet[2]) ||
                   !eat_uint(f, tet[3]) ||
                   !eat_int(f, l)) assert(false && "failed reading tet");

                for(uint & vid : tet) vid -= 1;
                polys.push_back(tet);
                poly_labels.push_back(l);
                p_unique_labels.insert(l);
            }
        }
        else if(strcmp(cell_type, "Hexahedra")==0)
        {
            if(!eat_int(f, nc)) assert(false && "failed reading num hexa");
            for(int i=0; i<nc; ++i)
            {
                int l;
                std::vector<uint> hex(8);                
                if(!eat_uint(f, hex[0]) ||
                   !eat_uint(f, hex[1]) ||
                   !eat_uint(f, hex[2]) ||
                   !eat_uint(f, hex[3]) ||
                   !eat_uint(f, hex[4]) ||
                   !eat_uint(f, hex[5]) ||
                   !eat_uint(f, hex[6]) ||
                   !eat_uint(f, hex[7]) ||
                   !eat_int(f, l)) assert(false && "failed reading hexa");

                for(uint & vid : hex) vid -= 1;
                polys.push_back(hex);
                poly_labels.push_back(l);
                p_unique_labels.insert(l);
            }
        }
        else if(strcmp(cell_type, "Triangles")==0)
        {
            if(!eat_int(f, nc))  assert(false && "failed reading num tris");
            for(int i=0; i<nc; ++i)
            {
                int l;
                std::vector<uint> tri(4);
                if(!eat_uint(f, tri[0]) ||
                   !eat_uint(f, tri[1]) ||
                   !eat_uint(f, tri[2]) ||
                   !eat_int(f, l)) assert(false && "failed reading tri");
                // discard these elements
            }
        }
        else if(strcmp(cell_type, "Quadrilaterals")==0)
        {
            if(!eat_int(f, nc))  assert(false && "failed reading num quads");
            for(int i=0; i<nc; ++i)
            {
                int l;
                std::vector<uint> quad(4);
                if(!eat_uint(f, quad[0]) ||
                   !eat_uint(f, quad[1]) ||
                   !eat_uint(f, quad[2]) ||
                   !eat_uint(f, quad[3]) ||
                   !eat_int(f, l)) assert(false && "failed reading quad");
                // discard these elements
            }
        }
        else if(strcmp(cell_type, "Edges")==0)
        {
            if(!eat_int(f, nc)) assert(false && "failed reading num edges");
            for(int i=0; i<nc; ++i)
            {
                int l;
                std::vector<uint> edge(4);
                if(!eat_uint(f, edge[0]) ||
                   !eat_uint(f, edge[1]) ||
                   !eat_int(f, l)) assert(false && "failed reading edge");
                // discard these elements
            }
        }
        else if(strcmp(cell_type, "Corners")==0)
        {
            if(!eat_int(f, nc)) assert(false && "failed reading corners");
            for(int i=0; i<nc; ++i)
            {
                std::vector<uint> corner(4);
                if(!eat_uint(f, corner[0])) assert(false && "failed reading corner");
                // discard these elements
            }
        }
    }
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
