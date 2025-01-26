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

#include <cinolib/io/read_RF.h>
#include <iostream>
#include <map>

namespace cinolib
{

CINO_INLINE
void read_RF(const char *filename, std::vector<vec3d> &verts,
             std::vector<std::vector<uint>> &faces,
             std::vector<std::vector<uint>> &polys,
             std::vector<std::vector<bool>> &polys_winding) {
  setlocale(LC_NUMERIC,
            "en_US.UTF-8"); // makes sure "." is the decimal separator

  size_t lastindex = std::string(filename).find_last_of(".");
  std::string basename = std::string(filename).substr(0, lastindex);
  std::string n_node = basename + ".node";
  FILE *f_node = fopen(n_node.c_str(), "r");
  FILE *f_ele = fopen((basename + ".ele").c_str(), "r");
  if (!f_node || !f_ele) {
    std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__
              << " : load_RF() : couldn't open input file(s) " << basename
              << std::endl;
    exit(-1);
  }
  char buffer[100];
  fgets(buffer, 100, f_node); // ignore the initial line of both files
  fgets(buffer, 100, f_ele);

  uint nv, dim, fv0, fv1;
  fscanf(f_node, "%d %d %d %d", &nv, &dim, &fv0, &fv1);
  for (uint i = 0; i < nv; ++i) {
    double x, y, z;
    uint vid;
    fscanf(f_node, "%d %lf %lf %lf", &vid, &x, &y, &z);
    verts.push_back(vec3d(x, y, z));
  }
  fclose(f_node);

  std::map<std::vector<uint>, uint> f_map;
  uint fresh_id = 0;
  uint np, fp0;
  fscanf(f_ele, "%d %d", &np, &fp0);
  for (uint i = 0; i < np; ++i) {
    uint pid, nf;
    fscanf(f_ele, "%d %d", &pid, &nf);

    std::vector<uint> p;
    std::vector<bool> p_winding;
    for (uint j = 0; j < nf; ++j) {
      uint fid, nv;
      fscanf(f_ele, "%d %d", &fid, &nv);

      std::vector<uint> f;
      for (uint j = 0; j < nv; ++j) {
        uint vid;
        fscanf(f_ele, "%d", &vid);
        f.push_back(vid);
      }

      std::vector<uint> f2(f.size()); // f2 = face f with inverse winding
      std::reverse_copy(f.begin(), f.end(), f2.begin());

      auto it = f_map.find(f2);
      if (it != f_map.end()) { // f exists with inverse winding
        p.push_back(it->second);
        p_winding.push_back(false);
      } else { // f does not exist yet
        faces.push_back(f);
        f_map[f] = fresh_id;
        ++fresh_id;
        p.push_back(faces.size() - 1);
        p_winding.push_back(true);
      }
    }
    polys.push_back(p);
    polys_winding.push_back(p_winding);
  }
  fclose(f_ele);
}

} // namespace cinolib
