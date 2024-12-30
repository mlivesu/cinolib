/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2023: Federico Meloni                                           *
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
*     Federico Meloni (federico.meloni3@unica.it)                               *
*                                                                               *
*     University of Cagliari                                                    *
*     Via Ospedale, 72                                                          *
*     09124 Cagliari,                                                           *
*     Italy                                                                     *
*********************************************************************************/

#include <cinolib/io/read_NODE_ELE.h>
#include <cinolib/vector_serialization.h>
#include <cinolib/io/io_utilities.h>
#include <cinolib/quality_tet.h>
#include <iostream>
#include <unordered_set>

namespace cinolib
{

CINO_INLINE
void read_NODE_ELE(const char                     * filename,
                   std::vector<vec3d>             & verts,
                   std::vector<std::vector<uint>> & polys,
                   std::vector<int>               & vert_labels,
                   std::vector<int>               & poly_labels)
{
    verts.clear();
    polys.clear();
    vert_labels.clear();
    poly_labels.clear();

    setlocale(LC_NUMERIC, "en_US.UTF-8");

    FILE *f = fopen(filename, "r");
    if(!f) {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_NODE_ELE() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    std::unordered_set<int> v_unique_labels;
    std::unordered_set<int> p_unique_labels;

    //read header .node file
    int nc, dim, nan, nbm; //node count, dimensions, no attribute (nodes), no boundary marker
    eat_int(f, nc);
    eat_int(f, dim);
    eat_int(f, nan);
    eat_int(f, nbm);

    //get verts from .node file
    int vid, l;
    double x=0, y=0, z=0;
    for(int i=0; i<nc; i++) {

        if(!eat_int(f, vid) || !eat_double(f, x) || !eat_double(f, y) || !eat_double(f, z))
            assert(false && "failed reading vert");
        if(nbm == 1)
            eat_int(f, l);
        verts.push_back(vec3d(x, y, z));
        vert_labels.push_back(l);
        v_unique_labels.insert(l);

    }
    fclose(f);

    //check
    if(verts.size() != nc)
        assert(false && "failed reading verts");

    //get .ele file
    std::string file_ele(filename);
    std::size_t pos = file_ele.find(".node");
    file_ele = file_ele.substr(0, pos);
    file_ele += ".ele";
    f = fopen(file_ele.c_str(), "r");

    if(!f) {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_NODE_ELE() : couldn't open input file " << file_ele << std::endl;
        exit(-1);
    }

    //read header .ele file
    int nt, npt, nat; //num tetrahedra, nodes per tetrahedron, num attributes (tetrahedra)
    eat_int(f, nt);
    eat_int(f, npt);
    eat_int(f, nat);

    //get tets from .ele file
    uint pid, tet[4];
    for(int i=0; i<nt; i++) {
        if(!eat_uint(f, pid) || !eat_uint(f, tet[0]) || !eat_uint(f, tet[1]) || !eat_uint(f, tet[2]) || !eat_uint(f, tet[3]))
            assert(false && "failed reading tets");
        for(int j=0; j<4; j++)
            tet[j] -= 1;
        if(tet_scaled_jacobian(verts[tet[0]],verts[tet[1]],verts[tet[2]],verts[tet[3]]) < 0)
            std::swap(tet[0], tet[1]);

        polys.push_back({tet[0],tet[1],tet[2],tet[3]});
    }

    fclose(f);

}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_NODE_ELE(const char                     * filename,
                   std::vector<vec3d>             & verts,
                   std::vector<std::vector<uint>> & polys)
{
    std::vector<int> vert_labels, poly_labels;
    read_NODE_ELE(filename, verts, polys, vert_labels, poly_labels);
}

}
