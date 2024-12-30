/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2024: Federico Meloni                                           *
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

#include <cinolib/io/read_OVM.h>
#include <cinolib/io/io_utilities.h>

namespace cinolib
{

CINO_INLINE
void read_OVM(const char *filename, std::vector<vec3d> &verts, std::vector<uint> &edges, std::vector<std::vector<uint>> &faces, std::vector<uint> &polys) {

    int vid, eid, fid, pid, trash;
    std::vector<uint> face;
    std::vector<uint> poly;
    bool clockwise, reverse_edge;

    verts.clear();
    edges.clear();
    faces.clear();
    polys.clear();

    setlocale(LC_NUMERIC, "en_US.UTF-8");

    FILE *f = fopen(filename, "r");

    if(!f) {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_OVM() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    //read header .ovm file
    int vc, ec, fc, pc; //vertex count, edge count, face count, poly count

    //get verts from .ovm file
    seek_keyword(f, "Vertices");
    eat_int(f, vc);
    assert(verts.empty());
    double x=0, y=0, z=0; //coords
    for(vid=0; vid<vc; vid++) {
        if(!eat_double(f, x) || !eat_double(f, y) || !eat_double(f, z)) assert(false && "failed reading vert");
        verts.push_back(vec3d(x, y, z));
    }
    assert(vid == vc);
    assert(verts.size() == vc);

    //get edges from .ovm file
    seek_keyword(f, "Edges");
    eat_int(f, ec);
    assert(edges.empty());
    int v0, v1; //edge vertices
    for(eid=0; eid<ec; eid++) {
        if(!eat_int(f, v0) || !eat_int(f, v1))
            assert(false && "failed reading edge");
        edges.push_back(v0);
        edges.push_back(v1);

        //check the vertices of the edge are different
        if(v0 == v1) std::cout << "BAD EDGE -> " << v0 << " " << v1 << std::endl;
    }
    assert(eid == ec);
    assert(edges.size() == ec*2);

    //get ovm_faces from .ovm file
    seek_keyword(f, "Faces");
    eat_int(f, fc);
    assert(faces.empty());
    for(fid=0; fid<fc; fid++) {

        //get the number of edges in a face
        eat_int(f, trash);
        //get face
        face.clear();

        //get the first edge id
        eat_int(f, eid);
        reverse_edge = eid % 2;         //check if the edge is reversed
        eid = (eid - reverse_edge)/2;   //get the real edge id

        //put the edge in the face
        face.push_back(edges[eid * 2 + 0]);
        face.push_back(edges[eid * 2 + 1]);
        if(reverse_edge) std::swap(face[0], face[1]);

        //get the second edge id
        eat_int(f, eid);
        reverse_edge = eid % 2;
        eid = (eid - reverse_edge)/2;
        if(edges[eid * 2 + 0] != face[0] && edges[eid * 2 + 0] != face[1]) face.push_back(edges[eid * 2 + 0]);
        else face.push_back(edges[eid * 2 + 1]);

        //last edge is useless
        eat_int(f, trash);
        //save the face
        faces.push_back(face);

        //check the vertices of the face are different
        if(face[0] == face[1] || face[0] == face[2] || face[1] == face[2]) std::cout << "BAD FACE -> " << face[0] << " " << face[1] << " " << face[2] << std::endl;

    }
    assert(fid == fc);
    assert(faces.size() == fc);

    //get polys from .ovm file
    seek_keyword(f, "Polyhedra");
    eat_int(f, pc);
    assert(polys.empty());
    for(pid=0; pid<pc; pid++) {

        //get the number of faces in a poly
        eat_int(f, trash);
        assert(trash == 4);

        //get the first face id
        eat_int(f, fid);
        clockwise = fid % 2;
        fid = (fid - clockwise)/2;

        //get the first 3 verts
        polys.push_back(faces[fid][0]);
        polys.push_back(faces[fid][1]);
        polys.push_back(faces[fid][2]);
        if(clockwise) std::swap(polys[polys.size()-3], polys[polys.size()-2]);

        //get the second face id
        eat_int(f, fid);
        clockwise = fid % 2;
        fid = (fid - clockwise)/2;
        //get the last vert
        assert((polys.size()+1)%4 == 0);
        if(faces[fid][0] != polys[polys.size()-3] && faces[fid][0] != polys[polys.size()-2] && faces[fid][0] != polys[polys.size()-1]) polys.push_back(faces[fid][0]);
        else if(faces[fid][1] != polys[polys.size()-3] && faces[fid][1] != polys[polys.size()-2] && faces[fid][1] != polys[polys.size()-1]) polys.push_back(faces[fid][1]);
        else polys.push_back(faces[fid][2]);
        assert(polys.size()%4 == 0);

        //last two faces are useless
        eat_int(f, trash);
        eat_int(f, trash);

        //check the last 4 verts are different
        if(polys[polys.size()-4] == polys[polys.size()-3] || polys[polys.size()-4] == polys[polys.size()-2] || polys[polys.size()-4] == polys[polys.size()-1] ||
           polys[polys.size()-3] == polys[polys.size()-2] || polys[polys.size()-3] == polys[polys.size()-1] ||
           polys[polys.size()-2] == polys[polys.size()-1]) std::cout << "BAD POLY -> " << polys[polys.size()-4] << " " << polys[polys.size()-3] << " " << polys[polys.size()-2] << " " << polys[polys.size()-1] << std::endl;
    }
    assert(pid == pc);
    assert(polys.size() == pc*4);

    /*
    std::cout << "Reading OVM file..." << std::endl;
    std::cout << "Verts......." << vc << std::endl;
    std::cout << "Edges......." << ec << std::endl;
    std::cout << "Faces......." << fc << std::endl;
    std::cout << "Polys......." << pc << std::endl;
    std::cout << "====================" << std::endl;
    */

    //close file
    fclose(f);
}

}