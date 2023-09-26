/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2019: Tommaso Sorgente
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
*     Tommaso Sorgente (tommaso.sorgente@.cnr.it)                         *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/

#include <cinolib/io/read_OVM.h>
#include <iostream>
#include <map>

namespace cinolib
{

CINO_INLINE
void read_OVM(const char *filename, std::vector<vec3d> &verts,
              std::vector<std::vector<uint>> &faces,
              std::vector<std::vector<uint>> &polys,
              std::vector<std::vector<bool>> &polys_winding) {
  setlocale(LC_NUMERIC,
            "en_US.UTF-8"); // makes sure "." is the decimal separator

  FILE *file = fopen(filename, "r");
  if (!file) {
    std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__
              << " : load_RF() : couldn't open input file(s) " << filename
              << std::endl;
    exit(-1);
  }
  char buffer[100];
  fgets(buffer, 100, file); // ignore the initial line

  fgets(buffer, 100, file); // #Vertices
  uint nv;
  fscanf(file, "%d", &nv);
  for (uint i = 0; i < nv; ++i) {
    double x, y, z;
    fscanf(file, "%lf %lf %lf", &x, &y, &z);
    verts.push_back(vec3d(x, y, z));
  }

  fgets(buffer, 100, file); // #Edges
  fgets(buffer, 100, file);
  uint ne;
  fscanf(file, "%d", &ne);
  std::vector<std::pair<uint, uint>> edges(2 * ne);
  for (uint i = 0; i < ne; ++i) {
    uint vid0, vid1;
    fscanf(file, "%d %d", &vid0, &vid1);
    edges.at(2 * i) = std::pair<uint, uint>(vid0, vid1);
    edges.at(2 * i + 1) = std::pair<uint, uint>(vid1, vid0);
  }

  fgets(buffer, 100, file); // #Faces
  fgets(buffer, 100, file);
  uint nf;
  fscanf(file, "%d", &nf);
  for (uint i = 0; i < nf; ++i) {
    uint s; // size of the face
    fscanf(file, "%d", &s);
    std::vector<uint> f(s);
    for (uint j = 0; j < s; ++j) {
      uint eid;
      fscanf(file, "%d", &eid);
      f.at(j) = edges.at(eid).first;
    }
    faces.push_back(f);
  }

  fgets(buffer, 100, file); // #Polyhedra
  fgets(buffer, 100, file);
  uint np;
  fscanf(file, "%d", &np);
  for (uint i = 0; i < np; ++i) {
    uint s; // size of the polyhedron
    fscanf(file, "%d", &s);
    std::vector<uint> p(s);
    std::vector<bool> pw(s);
    for (uint j = 0; j < s; ++j) {
      uint fid;
      fscanf(file, "%d", &fid);
      if (fid % 2 == 0) {
        p.at(j) = fid / 2;
        pw.at(j) = true;
      } else {
        p.at(j) = (fid - 1) / 2;
        pw.at(j) = false;
      }
    }
    polys.push_back(p);
    polys_winding.push_back(pw);
  }
  fclose(file);
}

} // namespace cinolib
